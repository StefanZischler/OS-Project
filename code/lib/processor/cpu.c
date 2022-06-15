#include <stdio.h>
#include <stdbool.h>

#include <cpu.h>
#include <instruction_set.h>
#include <interrupts.h>
#include <emulator.h>
#include <bus.h>
#include <stack.h>
#include <ppu.h>


cpu_context ctx = {0};

cpu_context* cpu_get_context() {
  return &ctx;
}

void cpu_init() {
  //set program counter to initial value
  ctx.registers.pc = 0x100;
  
  //set stack pointer to initial value
  ctx.registers.sp = 0xFFFE;
  
  //set all registers to initial value
  *((short *)&ctx.registers.a) = 0xB001;
  *((short *)&ctx.registers.b) = 0x1300;
  *((short *)&ctx.registers.c) = 0xD800;
  *((short *)&ctx.registers.d) = 0x4D01;

  //print values for debugging
  //printf("pc: %d\n", ctx.registers.pc);
  //printf("sp: %d\n", ctx.registers.sp);
  //printf("Z-flag: %d\n", cpu_flag_Z());
  //printf("N-flag: %d\n", cpu_flag_N());
  //printf("H-flag: %d\n", cpu_flag_H());
  //printf("C-flag: %d\n", cpu_flag_C());

  ctx.interrupt_master_enabled_flag = false;
  ctx.ie_register = 0;
  ctx.interrupt_flag = 0;
  
  ppu_init();
  
}

bool cpu_flag_Z() {
  return (ctx.registers.f & (1 << 7)) >> 7;
}

bool cpu_flag_N() {
  return (ctx.registers.f & (1 << 6)) >> 6;
}

bool cpu_flag_H() {
  return (ctx.registers.f & (1 << 5)) >> 5;
}

bool cpu_flag_C() {
  return (ctx.registers.f & (1 << 4)) >> 4;
}

void cpu_set_flags(bool z, bool n, bool h, bool c) {
  cpu_set_flag(z, 7);
  cpu_set_flag(n, 6);
  cpu_set_flag(h, 5);
  cpu_set_flag(c, 4);
}

void cpu_set_flag(bool n, int offset) {
  if(n) {
    //set flag to true
    ctx.registers.f = ctx.registers.f | (1 << offset);
  } else {
    //set flag to false
    ctx.registers.f = ctx.registers.f & ~(1 << offset);
  }
}

//helper function to look up if Z/C Flags are set
static bool check_flag(cpu_context *ctx){
  bool z_flag = cpu_flag_Z;
  bool c_flag = cpu_flag_C;

  switch(ctx->current_instruction->flag) {
    case FL_NONE: return true;
    case FL_C: return c_flag;
    case FL_NC: return !c_flag;
    case FL_Z: return z_flag;
    case FL_NZ: return !z_flag;
  }

  return false;
}

u8 cpu_get_ie_register() {
  return ctx.ie_register;
}

void cpu_set_ie_register(u8 value) {
  ctx.ie_register = value;
}


/* 
HELPER FUNCTIONS
*/
//reverse the input bits (register data) -> needed for the 16-bit registers
u16 reverse_bits(u16 bits) {
    return ((bits & 0xFF00) >> 8) | ((bits = 0x00FF) << 8);
}

//read from register defined by instruction
u16 instruction_read_register(register_type register_type) {
    switch(register_type) {
        //8-bit registers
        case REG_A: return ctx.registers.a;
        case REG_B: return ctx.registers.b;
        case REG_C: return ctx.registers.c;
        case REG_D: return ctx.registers.d;
        case REG_E: return ctx.registers.e;
        case REG_H: return ctx.registers.h;
        case REG_L: return ctx.registers.l;
        
        //16-bit registers
        case REG_AF: return reverse_bits(*((u16 *)&ctx.registers.a));
        case REG_BC: return reverse_bits(*((u16 *)&ctx.registers.b));
        case REG_DE: return reverse_bits(*((u16 *)&ctx.registers.d));
        case REG_HL: return reverse_bits(*((u16 *)&ctx.registers.h));
        //stack pointer
        case REG_SP: return ctx.registers.sp;
        //program counter
        case REG_PC: return ctx.registers.pc;
        //default
        default: return 0;
    }
}

//set value into register given by instruction
u16 instruction_set_register(register_type register_type, u16 value) {
    switch(register_type) {
        //8-bit registers
        case REG_A: ctx.registers.a = value & 0xFF; break;
        case REG_B: ctx.registers.b = value & 0xFF; break;
        case REG_C: ctx.registers.c = value & 0xFF; break;
        case REG_D: ctx.registers.d = value & 0xFF; break;
        case REG_E: ctx.registers.e = value & 0xFF; break;
        case REG_H: ctx.registers.h = value & 0xFF; break;
        case REG_L: ctx.registers.l = value & 0xFF; break;
        
        //16-bit registers
        case REG_AF: *((u16 *)&ctx.registers.a) = reverse_bits(value);break;
        case REG_BC: *((u16 *)&ctx.registers.b) = reverse_bits(value);break;
        case REG_DE: *((u16 *)&ctx.registers.d) = reverse_bits(value);break;
        case REG_HL: *((u16 *)&ctx.registers.h) = reverse_bits(value);break;
        //stack pointer
        case REG_SP: ctx.registers.sp = value & 0xFF; break;
        //program counter
        case REG_PC: ctx.registers.pc = value & 0xFF; break;
        //default
        default: break;
    }
}

/*
0xCB instruction set
*/
u16 cb_read_register(register_type register_type) {
    switch(register_type) {
        //8-bit registers
        case REG_A: return ctx.registers.a;
        case REG_F: return ctx.registers.f;
        case REG_B: return ctx.registers.b;
        case REG_C: return ctx.registers.c;
        case REG_D: return ctx.registers.d;
        case REG_E: return ctx.registers.e;
        case REG_H: return ctx.registers.h;
        case REG_L: return ctx.registers.l;
        
        //16-bit registers
        case REG_HL: return bus_read(instruction_read_register(REG_HL));
        
        //default
        default: return 0;
    }
}

//special helper function for 0xCB instruction set
u16 cb_set_register(register_type reg_type, u16 value) {
    switch(reg_type) {
        //8-bit registers
        case REG_A: ctx.registers.a = value & 0xFF; break;
        case REG_F: ctx.registers.f = value & 0xFF; break;
        case REG_B: ctx.registers.b = value & 0xFF; break;
        case REG_C: ctx.registers.c = value & 0xFF; break;
        case REG_D: ctx.registers.d = value & 0xFF; break;
        case REG_E: ctx.registers.e = value & 0xFF; break;
        case REG_H: ctx.registers.h = value & 0xFF; break;
        case REG_L: ctx.registers.l = value & 0xFF; break;
        
        //16-bit registers
        case REG_HL: bus_write(instruction_read_register(REG_HL), value);break;
      
        //default
        default: break;
    }
}

//function to jump to a specific address in program counter, used for example in proc_jr function
static void jump_to_address (cpu_context *ctx, u16 address, bool is_data_pushed_pc) {
  if (check_flag(ctx)) {
    if(is_data_pushed_pc) {
      emulator_cycles(2);
      stack_push16(ctx->registers.pc);
    }
    ctx->registers.pc = address;
    emulator_cycles(1);
  }
}

/* 
FETCHING
*/
void fetch_instruction () {
    ctx.current_opcode = bus_read(ctx.registers.pc++);
    ctx.current_instruction = get_instruction_opcode(ctx.current_opcode);
};


// read opcode from bus and get instruction from instruction_set
void fetch_instruction_data() {
    ctx.memory_destination = 0;
    ctx.destination_is_in_memory = false;

    // test if there is current istruction
    // return if not
    if(ctx.current_instruction == NULL) {
        return;
    }

    //switch between all addressing component cases
    switch (ctx.current_instruction->components) {
      //No instruction
      case AC_NONE:
          return;
      //read 16-bit data (and save to a register)
      case AC_R_D16:
      case AC_D16: {
          //can only read 8-bits at a time thats why split 16-bit into lower and higher 8-bits
          //read lower 8-bit from program counter
          u16 low = bus_read(ctx.registers.pc);
          emulator_cycles(1);

          //read higher 8-bit from program counter
          u16 high = bus_read(ctx.registers.pc + 1);
          emulator_cycles(1);

          //fetch the combined data from lower and higher bit
          ctx.fetched_data = low | (high << 8);
          //increase the program counter by 2
          ctx.registers.pc += 2;
          
          return;
      }
      //read from second register
      case AC_R_R:
          ctx.fetched_data = instruction_read_register(ctx.current_instruction->register_2);
          return;

      //read from register 1 and look up memory destination from register 2
      case AC_MR_R:
          ctx.fetched_data = instruction_read_register(ctx.current_instruction->register_2);
          ctx.memory_destination = instruction_read_register(ctx.current_instruction->register_1);
          ctx.destination_is_in_memory = true;

          if (ctx.current_instruction->register_1 == REG_C) {
              ctx.memory_destination |= 0xFF00;
          }
          return;

      //read from first register
      case AC_R:
          ctx.fetched_data = instruction_read_register(ctx.current_instruction->register_1);
          return;

      //read the d8 value from bus and increas program counter
      case AC_R_D8:
          ctx.fetched_data = bus_read(ctx.registers.pc);
          emulator_cycles(1);
          ctx.registers.pc++;
          return;

      //read memory location specified by another register
      case AC_R_MR:{
        u16 address = instruction_read_register(ctx.current_instruction->register_2);
        if (ctx.current_instruction->register_2 == REG_C) {
          address |= 0xFF00;
        }
        ctx.fetched_data = bus_read(address);
        emulator_cycles(1);
      }  return;

      //read the data from memory specified by register HL and set it in another  register. 
      //simultaniously incriment HL
      case AC_R_HLI:
        ctx.fetched_data = bus_read(instruction_read_register(ctx.current_instruction->register_2));
        emulator_cycles(1);
        instruction_set_register(REG_HL, instruction_read_register(REG_HL) +1);
        return; 

       //read the data from memory specified by register HL and set it in another  register. 
      //simultaniously decrement HL
      case AC_R_HLD:
        ctx.fetched_data = bus_read(instruction_read_register(ctx.current_instruction->register_2));
        emulator_cycles(1);
        instruction_set_register(REG_HL, instruction_read_register(REG_HL) -1);
        return; 

      //read amd store the data from a register to a memory location specified by register HL. 
      //simultaniously incriment HL
      case AC_HLI_R:
        ctx.fetched_data = bus_read(instruction_read_register(ctx.current_instruction->register_2));
        ctx.memory_destination = instruction_read_register(ctx.current_instruction->register_1);
        ctx.destination_is_in_memory = true;
        instruction_set_register(REG_HL, instruction_read_register(REG_HL) +1);
        return; 

      //read amd store the data from a register to a memory location specified by register HL. 
      //simultaniously decrement HL
      case AC_HLD_R:
        ctx.fetched_data = bus_read(instruction_read_register(ctx.current_instruction->register_2));
        ctx.memory_destination = instruction_read_register(ctx.current_instruction->register_1);
        ctx.destination_is_in_memory = true;
        instruction_set_register(REG_HL, instruction_read_register(REG_HL) -1);
        return; 

      //load contents from internal RAM (0xFF00-0xFFFF) into register A
      case AC_R_A8:
        ctx.fetched_data = bus_read(ctx.registers.pc);
        emulator_cycles(1);
        ctx.registers.pc++;
        return; 

      //store contents of register A into internal RAM (0xFF00-0xFFFF)
      case AC_A8_R:
        ctx.fetched_data = bus_read(ctx.registers.pc) | 0xFF00;
        ctx.destination_is_in_memory = true;
        emulator_cycles(1);
        ctx.registers.pc++;
        return; 

      //read and add contents of SP to HL
      case AC_HL_SPR:
        ctx.fetched_data = bus_read(ctx.registers.pc);
        emulator_cycles(1);
        ctx.registers.pc++;
        return; 

      //read d8 value
      case AC_D8:
        ctx.fetched_data = bus_read(ctx.registers.pc);
        emulator_cycles(1);
        ctx.registers.pc++;
        return; 

      //save data from 8-bit operand d8 to a memory location specified by HL
      case AC_MR_D8:
        ctx.fetched_data = bus_read(ctx.registers.pc);
        emulator_cycles(1);
        ctx.registers.pc++;
        ctx.memory_destination = instruction_read_register(ctx.current_instruction->register_1);
        ctx.destination_is_in_memory = true;
        return; 
        
       //read from memory register HL 
      case AC_MR:
        ctx.memory_destination = instruction_read_register(ctx.current_instruction->register_1);
        ctx.destination_is_in_memory = true;
        ctx.fetched_data = bus_read(instruction_read_register(ctx.current_instruction->register_1));
        emulator_cycles(1);
        return;

      //save or write from/to a 16-bit address from a register  
      case AC_A16_R:
      case AC_D16_R: {
        //can only read 8-bits at a time thats why split 16-bit into lower and higher 8-bits
        //read lower 8-bit from program counter
        u16 low = bus_read(ctx.registers.pc);
        emulator_cycles(1);

        //read higher 8-bit from program counter
        u16 high = bus_read(ctx.registers.pc + 1);
        emulator_cycles(1);

        //fetch the combined data from lower and higher bit into memory destination
        ctx.memory_destination = low | (high << 8);
        ctx.destination_is_in_memory = true;
        //increase the program counter by 2
        ctx.registers.pc += 2;
        ctx.fetched_data = instruction_read_register(ctx.current_instruction->register_2);
      } return;
      
      //read 16-bit address to load to program counter or start at address
      case AC_R_A16: {
        //read lower 8-bit from program counter
        u16 low = bus_read(ctx.registers.pc);
        emulator_cycles(1);

        //read higher 8-bit from program counter
        u16 high = bus_read(ctx.registers.pc + 1);
        emulator_cycles(1);

        //fetch the combined data from lower and higher bit
        u16 address = low | (high << 8);
        //increase the program counter by 2
        ctx.registers.pc += 2;
        ctx.fetched_data = bus_read(address);
        emulator_cycles(1);
      } return;  
    
      default:
          printf("Unknown Addressing Components! %d\n", ctx.current_instruction->components);
          exit(-3);
          return;
    }
};

/* 
PROCESSING
*/

//execute the current instruction -> for that get the instruction type
static void execute() {
  printf("Executing instruction: %02X  PC: %04X\n", ctx.current_opcode, ctx.registers.pc);

  // get instruction type
    INS_TYPE type = instruction_get_type(ctx.current_instruction->type);
  // error message if this instruction type is yet not supported
  if(!type) {
    printf("Type not supported yet");
  }
  // execute instruction
  type(&ctx);
  
}

/*basic cpu step
 1. get instruction
 2. get data from registers/bus
 3. execute instruction
*/
bool cpu_step () {
  if(!ctx.halted) {
    u16 pc = ctx.registers.pc;

    fetch_instruction();
    emulator_cycles(1);
    fetch_instruction_data();
    // for debugging instructions
    printf("%04X:  (%02X %02X %02X) A: %02X B: %02X C: %02X\n",
      pc, ctx.current_opcode, bus_read(pc + 1), bus_read(pc + 2),
      ctx.registers.a, ctx.registers.b, ctx.registers.c);
    
    if(ctx.current_instruction == NULL) {
      printf("Instruction %02X not implemented yet...\n", ctx.current_opcode);
      exit(-5);
    }
    execute();
  } else {
    //CPU is halted
    emulator_cycles(1);
    if(ctx.interrupt_flag) {
      ctx.halted = false;
    }
  }
  if (ctx.interrupt_master_enabled_flag) {
    cpu_handle_interrupts(&ctx);
    ctx.enable_interrupt_master = false;
  }
  if (ctx.enable_interrupt_master) {
    ctx.interrupt_master_enabled_flag = true;
  }

  return true;
}

//if no instruction this function should be called
static void type_none(cpu_context *ctx) {
  printf("Not a real instruction...\n");
  exit(-2);
}

//NOP instruction -> does nothing
static void type_nop(cpu_context *ctx) {
  //This function should do nothing
  printf("NOP Instruction\n");
}

//load either a d8/d16 value or from another register 2 into a register 1, special case with memory register HL
static void type_ld(cpu_context *ctx) {
  printf("LOAD Instruction\n");

  if (ctx->destination_is_in_memory) {
    //LD A, (BC) for example

    //test if second register is a 16-bit register
    if (ctx->current_instruction->register_2 >= REG_BC) {
      emulator_cycles(1);
      bus_write_16(ctx->memory_destination, ctx->fetched_data);
    } else {
      // 8-bit register
      bus_write(ctx->memory_destination, ctx->fetched_data);
    }
    emulator_cycles(1);
    return;
  }
  //special case with HL_SP 
  if (ctx->current_instruction->components == AC_HL_SPR) {
      //test if H flag needs to be set
      u8 h_flag = (instruction_read_register(ctx->current_instruction->register_2) & 0xF) 
      + (ctx->fetched_data & 0xF) >= 0x10;
      //test if C flag needs to be set
      u8 c_flag = (instruction_read_register(ctx->current_instruction->register_2) & 0xFF) 
      + (ctx->fetched_data & 0xFF) >= 0x100;
      //set H and C flag
      cpu_set_flags(0,0, h_flag, c_flag);
      //add 8-bit opperand to register HL
      instruction_set_register(ctx->current_instruction->register_1,
       instruction_read_register(ctx->current_instruction->register_2) + (int8_t) ctx->fetched_data);
      return;
  }

  //read data from second register from bus via fetch_data and load it into first register
  instruction_set_register(ctx->current_instruction->register_1, ctx->fetched_data);
    
}

//incriment the value in register
static void type_inc(cpu_context *ctx) {
  printf("Incriment Instruction\n");
  //get current value of register and increment it by 1
  u16 value = instruction_read_register(ctx->current_instruction->register_1) + 1;

  //if 16-bit register involved needs an extra emulator cycle
  if (ctx->current_instruction->register_2 >= REG_BC) {
      emulator_cycles(1);
    }
  
  //special case with memory register HL
  if (ctx->current_instruction->register_1 == REG_HL && ctx->current_instruction->components == AC_MR) {
    //read, incriment value by 1 and then write value from HL to bus
    value = bus_read(instruction_read_register(REG_HL)) + 1;
    value &= 0xFF;
    bus_write(instruction_read_register(REG_HL), value);
  } else {
    //load value into registers (not HL)
    instruction_set_register(ctx->current_instruction->register_1, value);
  }

  //set flags if current opcode is not in 0x03 column
  if((ctx->current_opcode & 0x03) == 0x03) {
    return;
  }
  cpu_set_flags(value == 0, 0, (value & 0x0F) == 0, -1);
  
}

//decriment the value in register
static void type_dec(cpu_context *ctx) {
  printf("Decriment Instruction\n");
  //get current value of register and decriment it by 1
  u16 value = instruction_read_register(ctx->current_instruction->register_1) - 1;

  //if 16-bit register involved needs an extra emulator cycle
  if (ctx->current_instruction->register_2 >= REG_BC) {
      emulator_cycles(1);
  }
  
  //special case with memory register HL
  if (ctx->current_instruction->register_1 == REG_HL && ctx->current_instruction->components == AC_MR) {
    //read, decriment value by 1 and then write value from HL to bus
    value = bus_read(instruction_read_register(REG_HL)) - 1;
    value &= 0xFF;
    bus_write(instruction_read_register(REG_HL), value);
  } else {
    //load value into registers (not HL)
    instruction_set_register(ctx->current_instruction->register_1, value);
  }

  //set flags if current opcode is not in 0x03 column
  if((ctx->current_opcode & 0x03) == 0x03) {
    return;
  }
  cpu_set_flags(value == 0, 0, (value & 0x0F) == 0, -1);
}

//rotate contents of register A to the left and update flag with value new bit 0 of register A
static void type_rlca(cpu_context *ctx) {
   printf("RLCA Instruction\n");
   
   //get 0 bit value
   u8 value = ctx->registers.a;
   //determine wether the CY flag needs to be set
   bool c_cond = (value >> 7) & 1;
   //rotate contents to the left and OR with c_condition
   value = (value << 1) | c_cond;
   //update contents of register A with new value
   ctx->registers.a = value;
   // set CY flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, value);
}

//add the contents of 8-bit operand d8 or from a register, into register A and update flags
static void type_add(cpu_context *ctx) {
  printf("ADD Instruction\n");
  //read data from register A and other register
  u32 data = instruction_read_register(ctx->current_instruction->register_1) + ctx->fetched_data;
  
  //define flag variables (for 8-bit instructions)
  bool z_flag = (data & 0xFF) == 0;
  bool n_flag = 0;
  bool h_flag = (instruction_read_register(ctx->current_instruction->register_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
  bool c_flag = (int)(instruction_read_register(ctx->current_instruction->register_1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) >= 0x100;
  
  //if 16-bit register involved needs an extra emulator cycle
  //get right flags for z,n,h,c
  if (ctx->current_instruction->register_2 >= REG_BC) {
    emulator_cycles(1);
    z_flag = -1;
    h_flag = (instruction_read_register(ctx->current_instruction->register_1) & 0xFFF) + (ctx->fetched_data & 0xFFF) >= 0x1000;
    c_flag = ((u32)instruction_read_register(ctx->current_instruction->register_1)) + ((u32)ctx->fetched_data) >= 0x10000;
  }
  
  //special case with SP register, loads a 8-bit s8 operand
  //get right flags for this case
  if (ctx->current_instruction->register_1 == REG_SP) {
    data = instruction_read_register(ctx->current_instruction->register_1) + (int8_t)ctx->fetched_data;
    z_flag = 0;
    h_flag = (instruction_read_register(ctx->current_instruction->register_1) & 0xF) + (ctx->fetched_data & 0xF) >= 0x10;
    c_flag = (int)(instruction_read_register(ctx->current_instruction->register_1) & 0xFF) + (int)(ctx->fetched_data & 0xFF) >= 0x100;
  }

  //set the register
  instruction_set_register(ctx->registers.a, data & 0xFFFF);
  //set the flags
  cpu_set_flags(z_flag, n_flag,  h_flag, c_flag);
  
}

//rotate contents of register A to the right and update flag with value new bit 0 of register A
static void type_rrc(cpu_context *ctx) {
  printf("RCC Instruction\n");
   
   //save first bit value
   u8 value = ctx->registers.a & 1;
   //rotate contents of A register to the right by one
   ctx->registers.a >>= 1;
   //OR contents of A with value (shifted to the last bit of A) to get C flag value
   ctx->registers.a |=  (value << 7);
   // set C flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, value);
}
static void type_stop(cpu_context *ctx) {
  fprintf(stderr, "Stopping CPU!\n");
}

//rotate contents of register A to the left by one and update 0-bit with contents of C flag
static void type_rla(cpu_context *ctx) {
  printf("RLA Instruction\n");
  //get register contents 
   u8 contents = ctx->registers.a;
   //get C flag contents
   bool c_flag = cpu_flag_C;
   //update C flag contents with first bit of register A
   u8 c_content = (contents >> 7) & 1;
   //rotate contents of A to the left by one and OR with c_flag 
   ctx->registers.a = (contents << 1) | c_flag;
   // set CY flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, c_content);
}

//if Z flag is set jump s8 steps from current address in program counter
static void type_jr(cpu_context *ctx) {
  printf("JR Instruction\n");
  // get the s8 steps from bus
  int8_t steps = (int8_t) (ctx->fetched_data & 0xFF);
  //calculate address (current pc + steps)
  u16 address = ctx->registers.pc + steps;
  //jump to address if Z flag is set
  jump_to_address(ctx, address, false);

} 


//rotate contents of register A to the right by one and update 0-bit with contents of C flag
static void type_rra(cpu_context *ctx) {
  printf("RRA Instruction\n");
  //save first bit value
   u8 value = ctx->registers.a & 1;
   //get C flag contents
   bool c_flag = cpu_flag_C;
   //rotate contents of A register to the right by one
   ctx->registers.a >>= 1;
   //OR contents of A with C flag (shifted to the last bit of A) to update A contents
   ctx->registers.a |=  (c_flag << 7);
   // set C flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, value);
}

//adjust register A after addition and subtraction operations,
//depends on which flag is set
static void type_daa(cpu_context *ctx) {
  printf("DDA Instruction\n");
  //initialize variables for updating reg A and flags
  u8 value = 0;
  bool c_flag = 0;
  //if H flag set and N flag not set, set value to 6
  if (cpu_flag_H || (!cpu_flag_N && (ctx->registers.a & 0xF) > 9)){
    value = 6;
  }  
  //if C flag set and N flag not set, OR value with 0x60 and set c_flag to 1
  if (cpu_flag_C || (!cpu_flag_N && (ctx->registers.a > 0x99))) {
    value |= 0x60;
    c_flag = 1;
  }

  //set flags
  cpu_set_flags(ctx->registers.a == 0, -1, 0, c_flag);
}

//take complement of reg A and set flags
static void type_cpl(cpu_context *ctx) {
  printf("CPL Instruction\n");
  //compare reg A with its complement
  ctx->registers.a = ~ctx->registers.a;
  //set flags
  cpu_set_flags(-1,1,1,-1);
}

//set the Carry Flag (C Flag)
static void type_scf(cpu_context *ctx) {
  printf("SCF Instruction\n");
  cpu_set_flags(-1,0,0,1);
}

//flip the Carry Flag (C Flag)
static void type_ccf(cpu_context *ctx) {
  printf("CCF Instruction\n");
  cpu_set_flags(-1,0,0, !cpu_flag_C);
}

//set CPU halted to true
static void type_halt(cpu_context *ctx) {
  printf("HALT Instruction\n");
  ctx->halted = true;
}

//add contents of register 2 and the C flag to the contents of register A and store it there
//set flags accordingly
static void type_adc(cpu_context *ctx) {
  printf("ADC Instruction\n");
  //get contents
  u16 data = ctx->fetched_data;
  u16 a_contents = ctx->registers.a;
  bool c_contents = cpu_flag_C;

  //update register A contents
  ctx->registers.a = (data + a_contents + c_contents) & 0xFF;

  //calculate 8-bit value if H and C flag are set
  bool h_flag = (data & 0xF) + (a_contents & 0xF) + c_contents > 0xF;
  bool c_flag = data + a_contents + c_contents > 0xFF;
  //set flags
  cpu_set_flags(ctx->registers.a == 0, 0, h_flag, c_flag);
}

//subtract contents of memory specified by register HL in register A
// and store results in register A
static void type_sub(cpu_context *ctx) {
  printf("SUB Instruction\n");
  //read data from register A and other register
  u32 data = instruction_read_register(ctx->current_instruction->register_1) - ctx->fetched_data;
  
  //define flag variables (for 8-bit instructions)
  bool z_flag = data == 0;
  bool n_flag = 1;
  bool h_flag = (instruction_read_register(ctx->current_instruction->register_1) & 0xF) - (ctx->fetched_data & 0xF) < 0;
  bool c_flag = ((int)instruction_read_register(ctx->current_instruction->register_1) & 0xFF) - ((int)ctx->fetched_data & 0xFF) < 0;

  //set the register
  instruction_set_register(ctx->current_instruction->register_1, data);
  //set the flags
  cpu_set_flags(z_flag, n_flag,  h_flag, c_flag);
}

//subtract contents of register 2 and the C flag to the contents of register A and store it there
//set flags accordingly
static void type_sbc(cpu_context *ctx) {
  printf("SBC Instruction\n");
  //read data from register A and other register
  long c_contents = (long) *cpu_flag_C;
  u8 data = ctx->fetched_data + c_contents;
  //define flag variables (for 8-bit instructions)
  bool z_flag = instruction_read_register(ctx->current_instruction->register_1) - data == 0;
  bool n_flag = 1;
  bool h_flag = ((int)instruction_read_register(ctx->current_instruction->register_1) & 0xF) - ((int)ctx->fetched_data & 0xF) - ((long) cpu_flag_C) < 0;
  bool c_flag = ((int)instruction_read_register(ctx->current_instruction->register_1) & 0xFF) - ((int)ctx->fetched_data & 0xFF) - ((long) cpu_flag_C) < 0;

  //set the register
  instruction_set_register(ctx->current_instruction->register_1, instruction_read_register(ctx->current_instruction->register_1) - data);
  //set the flags
  cpu_set_flags(z_flag, n_flag,  h_flag, c_flag);
}

//AND the contents of two register and save them to register A
static void type_and(cpu_context *ctx) {
  printf("AND Instruction\n");
  ctx->registers.a &= ctx->fetched_data & 0xFF;
  cpu_set_flags(ctx->registers.a == 0,0,1,0);
}

//XOR the contents of two register and save them to register A
static void type_xor(cpu_context *ctx) {
  printf("XOR Instruction\n");
  ctx->registers.a ^= ctx->fetched_data & 0xFF;
  cpu_set_flags(ctx->registers.a == 0,0,0,0);
}

//OR the contents of two register and save them to register A
static void type_or(cpu_context *ctx) {
  printf("OR Instruction\n");
  ctx->registers.a |= ctx->fetched_data & 0xFF;
  cpu_set_flags(ctx->registers.a == 0,0,0,0);
}

//Compare the contents of two register and set Z flag if they are equal
static void type_cp(cpu_context *ctx) {
  printf("CP Instruction\n");
  //calculate difference
  int difference = (int)ctx->registers.a - (int)ctx->fetched_data;
  //see if H flag needs to be set
  bool h_flag;
  if (((int)ctx->registers.a & 0x0F)- ((int)ctx->fetched_data & 0x0F) < 0) {
    h_flag = 1;
  } else {
    h_flag = 0;
  }
  
  cpu_set_flags(difference == 0,1, h_flag, difference < 0);
}

//depending if Z flag is set pop the program counter from stack and continue the subroutine
static void type_ret(cpu_context *ctx) {
  printf("RET Instruction\n");
  //check in no flag is set -> extra emulator cycle
  if (ctx->current_instruction->flag != FL_NONE) {
    emulator_cycles(1);
  }

  //check flag and pop from stack
  if (check_flag(ctx)) {
    u16 low = stack_pop8();
    emulator_cycles(1);
    u16 high = stack_pop8();
    emulator_cycles(1);

    u16 value = (high <<8) | low;
    ctx->registers.pc = value;
    emulator_cycles(1);
  }


  
}

//pop contents of stack into register pair
// 1.) load contents of Stack pointer into lower portion of register pair
// 2.) add 1 to Stack pointer and add new value to higher portion of register pair
// 3.) SP should at the end be 2 steps further 
static void type_pop(cpu_context *ctx) {
  printf("POP Instruction\n");
  u16 low = stack_pop8();
  emulator_cycles(1);
  u16 high = stack_pop8();
  emulator_cycles(1);

  u16 value = (high << 8) | low;

  instruction_set_register(ctx->current_instruction->register_1, value);

  //special case if AF register pair
  if(ctx->current_instruction->components == REG_AF) {
    instruction_set_register(ctx->current_instruction->register_1, value & 0xFFF0);
  }
  
}

//jump to a certain point in memory if the right flag is set
static void type_jp(cpu_context *ctx) {
  printf("JUMP Instruction\n");
  jump_to_address(ctx, ctx->fetched_data, false);
}

//tests if flag Z/C is 0/1 and the puts the next two bytes of program counter on stack
// and the a16-bit operand loaded into pc
static void type_call(cpu_context *ctx) {
  printf("CALL Instruction\n");
  jump_to_address(ctx, ctx->fetched_data, true);
}

//push contents of register pair on to stack
static void type_push(cpu_context *ctx) {
  printf("PUSH Instruction\n");
  u16 high = (instruction_read_register(ctx->current_instruction->register_1) >> 8) & 0xFF;
  emulator_cycles(1);
  stack_push8(high);
  u16 low = instruction_read_register(ctx->current_instruction->register_1) & 0xFF;
  emulator_cycles(1);
  stack_push8(low);

 emulator_cycles(1);
}

//push contents of pc on to stack and load first byte of 0x00 memory address -> 1 of 8 addresses
static void type_rst(cpu_context *ctx) {
  printf("RST Instruction\n");
  jump_to_address(ctx, ctx->current_instruction->memory_address, true);
}

//same as RET instruction but also enable instruction master flag
static void type_reti(cpu_context *ctx) {
  printf("RETI Instruction\n");
  ctx->interrupt_master_enabled_flag = true;
  type_ret(ctx);
}

//disable interrupt master flag
static void type_di(cpu_context *ctx) {
  printf("DI Instruction\n");
  ctx->interrupt_master_enabled_flag = false;
}

//enable interrupt master flag
static void type_ei(cpu_context *ctx) {
  printf("EI Instruction\n");
  ctx->enable_interrupt_master = true;
}

//load or store data to/from register A to a address in range 0xFF00-0xFFFF
//specified by register C
static void type_ldh(cpu_context *ctx) {
  printf("LDH Instruction\n");
  //if register 1 is A, load data from specified address into reg A, e.g. Ex3
  if (ctx->current_instruction->register_1 == REG_A) {
    instruction_set_register(ctx->current_instruction->register_1, bus_read(0xFF00 | ctx->fetched_data));
  } else {
    //if register 1 is not A, store data from A into specified address, e.g. Fx3
    bus_write(ctx->memory_destination, ctx->registers.a);
  }
  emulator_cycles(1);
}

//load contents of HL into program counter
//next instruction starts at address specified by new pc value
static void type_jphl(cpu_context *ctx) {
  printf("JPHL Instruction\n");
  instruction_set_register(ctx->current_instruction->register_1, ctx->registers.pc);
  bus_read(ctx->registers.pc);
}

//helper function to look up registers 
register_type standart_registers[] = {
  REG_A,
  REG_B,
  REG_C,
  REG_D,
  REG_E,
  REG_H,
  REG_L,
  REG_HL
};

//return correct register for bit operation
register_type get_register_type(u8 regi) {
  if (regi > 0b111) {
    return REG_NONE;
  }

  return standart_registers[regi];
}


//special instruction which has its own instruction table 
//16-bit opcodes where first 8-bit are 0xCB second 8-bit is new opcode
static void type_cb(cpu_context *ctx) {
  printf("Entering 0xCB \n");
  //get new opcode
  u8 new_opcode = ctx->fetched_data;
  //get register for new opcode, masked it with 0b111 to get register
  register_type regi = get_register_type(new_opcode & 0b111);
  //get instruction from new opcode x00-x3F
  u8 new_instruction = (new_opcode >> 3) & 0b111;
  //get which bit operation is called x40-xFF
  u8 bit_operations = (new_opcode >> 6) & 0b11;
  //read from register
  u8 register_value = cb_read_register(regi);

  emulator_cycles(1);

  //if memory register HL is used -> needs more emulator cycles
  if (regi == REG_HL) {
    emulator_cycles(2);
  }

  //new instructions 0x00-0x3F
  switch (new_instruction) {
    case 0: {
      //printf("RLC Instruction \n");
      //RLC
      //rotate contents of register to the left and set C flag depending on bit
      bool c_flag = false;
      u8 new_value = (register_value << 1) & 0xFF;

      if ((new_value & (1 << 7)) != 0) {
        new_value |= 1;
        c_flag = true;
      }

      cb_set_register(regi, new_value);
      cpu_set_flags(new_value == 0, false, false, c_flag);
    } return;

    case 1: {
      //printf("RRC Instruction \n");
      //RRC
      //rotate contents of register to the right and set flags accordingly
      u8 old_value = register_value;
      register_value >>= 1;
      register_value |= (old_value << 7);
      
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, old_value & 1);
      } return;

    case 2: {
      printf("RL Instruction \n");
      //RL
      //rotate contents of register to the left and copy contents of C flag into register
      //set flags accordingly
      u8 old_value = register_value;
      register_value <<= 1;
      register_value |= (long) *cpu_flag_C;
      
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, (old_value & 0x80));
    } return;

    case 3: {
      printf("RR Instruction \n");
      //RR
      //rotate contents of register to the right and copy contents of C flag into register
      u8 old_value = register_value;
      register_value >>= 1;
      register_value |= ((long)(*cpu_flag_C) << 7);
      
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, old_value & 1);
    } return;

    case 4: {
      printf("SLA Instruction \n");
      //SLA
      //rotate contents of register to the left and set C flag accordingly
      u8 old_value = register_value;
      register_value <<= 1;
      register_value |= (long) *cpu_flag_C;
      
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, (old_value & 0x80));
    } return;

    case 5: {
      printf("SRA Instruction \n");
      //SRA
      //rotate contents of register to the right and 
      //copy contents to C flag, bit 7 of register unchanged
      u8 old_value =  (int8_t)register_value >> 1;
     
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, old_value & 1);
    } return;

    case 6: {
      printf("SWAP Instruction \n");
      //SWAP
      //swap the contents of the lower 4-bit into the higher 4-bits and vice versa
      u8 low = ((register_value & 0xF0) >> 4);
      u8 high = ((register_value & 0xF) << 4);
      register_value = low | high;

      cb_set_register(regi, register_value);
      cpu_set_flags(register_value == 0, false, false, false);
    } return;

    case 7: {
      printf("SRL Instruction \n");
      //SRL
      //rotate contents of register to the right and set 
      // and contents of bit 0 are copied to C flag
      u8 old_value =  register_value >> 1;
     
      cb_set_register(regi, old_value);
      cpu_set_flags(!old_value, false, false, register_value & 1);
    } return;    
  }

  //bit operations
  switch (bit_operations)
  {
  case 1: {
    printf("BIT Instruction \n");
    //BIT
    //copy complement of bit 0 of register
    //change flags
    u8 complement = !(register_value & (1 << bit_operations));
    cpu_set_flags(complement, false, true, -1);
    return;
  }

  case 2:
    printf("RES Instruction \n");
    //RES
    //reset specified bit in register
    register_value &= ~(1 << bit_operations);
    cb_set_register(regi, register_value);
    return;

  case 3:
    printf("SET Instruction \n");
    //SET
    //set specified bit in register
    register_value |= (1 << bit_operations);
    cb_set_register(regi, register_value);
    return;
    
  default:
    return;
  }
  return;
}

// helper function to map instruction type from instruction set to a responding function 
static INS_TYPE type[] = {
    [INS_NONE] = type_none,
    [INS_NOP] = type_nop,
    [INS_LD]  = type_ld,
    [INS_INC] = type_inc,
    [INS_DEC] = type_dec,
    [INS_RLCA] = type_rlca,
    [INS_ADD] = type_add,
    [INS_RRC] = type_rrc,
    [INS_STOP]= type_stop,
    [INS_RLA] = type_rla,
    [INS_JR]  = type_jr,
    [INS_RRA] = type_rra,
    [INS_DAA] = type_daa,
    [INS_CPL] = type_cpl,
    [INS_SCF] = type_scf,
    [INS_CCF] = type_ccf,
    [INS_HALT] = type_halt,
    [INS_ADC] = type_adc,
    [INS_SUB] = type_sub,
    [INS_SBC] = type_sbc,
    [INS_AND] = type_and,
    [INS_XOR] = type_xor,
    [INS_OR]  = type_or,
    [INS_CP]  = type_cp,
    [INS_RET] = type_ret,
    [INS_POP] = type_pop,
    [INS_JP]  = type_jp,
    [INS_CALL] = type_call,
    [INS_PUSH] = type_push,
    [INS_RST] = type_rst,
    [INS_RETI] = type_reti,
    [INS_DI]  = type_di,
    [INS_EI]  = type_ei,
    [INS_LDH] = type_ldh,
    [INS_JPHL] = type_jphl,
    [INS_CB]  = type_cb
};


// get instruction type from helper function
INS_TYPE instruction_get_type(instruction_type ins_type) {
    return type[ins_type];
}


