#include <stdio.h>
#include <stdbool.h>

#include <cpu.h>
#include <instruction_set.h>
#include <interrupts.h>
#include <emulator.h>
#include <bus.h>


cpu_context ctx = {0};

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
  printf("pc: %d\n", ctx.registers.pc);
  printf("sp: %d\n", ctx.registers.sp);
  printf("Z-flag: %d\n", cpu_flag_Z());
  printf("N-flag: %d\n", cpu_flag_N());
  printf("H-flag: %d\n", cpu_flag_H());
  printf("C-flag: %d\n", cpu_flag_C());

  ctx.interrupt_master_enabled_flag = false;
  
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
  /*if(z) {
    //set flag to true
    ctx.registers.f = ctx.registers.f | (1 << 7);
  } else {
    //set flag to false
    ctx.registers.f = ctx.registers.f & ~(1 << 7);
  }
  if(n) {
    ctx.registers.f = ctx.registers.f | (1 << 6);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 6);
  }
  if(h) {
    ctx.registers.f = ctx.registers.f | (1 << 5);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 5);
  }
  if(c) {
    ctx.registers.f = ctx.registers.f | (1 << 4);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 4);
  }*/
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

//function to jump to a specific address in program counter, used for example in proc_jr function
static void jump_to_address (cpu_context *ctx, u16 address, bool is_data_pushed_pc) {
  if (check_flag(ctx)) {
    if(is_data_pushed_pc) {
      emulator_cycles(2);
      //TODO: stack push 16 address to register pc
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
    ctx.halted = false;
  }
  

  return true;
}

//requests an interrupt by setting the corresponding interrupt flag
void cpu_request_interrupt(interrupt_type type) {
  ctx.interrupt_flag = ctx.interrupt_flag | type;
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
    if (ctx->current_instance->register_2 >= REG_BC) {
      emulator_cycles(1);
      bus_write_16(ctx->memory_destination, ctx->fetched_data);
    } else {
      // 8-bit register
      bus_write(ctx->memory_destination, ctx->fetched_data);
    }
    emulator_cycles(1);
    return;
  }
  //special case with HL_SP not implemented yet -> not sure if necessary for tetris

  //read data from second register from bus via fetch_data and load it into first register
  instruction_set_register(ctx->current_instruction->register_1, ctx->fetched_data);
    
}

//incriment the value in register
static void type_inc(cpu_context *ctx) {
  printf("Incriment Instruction\n");
  //get current value of register and increment it by 1
  u16 value = instruction_read_register(ctx->current_instance->register_1) + 1;

  //if 16-bit register involved needs an extra emulator cycle
  if (ctx->current_instance->register_2 >= REG_BC) {
      emulator_cycles(1);
    }
  
  //special case with memory register HL
  if (ctx->current_instance->register_1 == REG_HL && ctx->current_instance->components == AC_MR) {
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
  u16 value = instruction_read_register(ctx->current_instance->register_1) - 1;

  //if 16-bit register involved needs an extra emulator cycle
  if (ctx->current_instance->register_2 >= REG_BC) {
      emulator_cycles(1);
    }
  
  //special case with memory register HL
  if (ctx->current_instance->register_1 == REG_HL && ctx->current_instance->components == AC_MR) {
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
static void type_add(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
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
   u8 c_flag = cpu_flag_C;
   //update C flag contents with first bit of register A
   u8 c_content = (contents >> 7) & 1;
   //rotate contents of A to the left by one and OR with c_flag 
   ctx->registers.a = (contents << 1) | c_flag;
   // set CY flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, c_content);
}

//if Z flag is set jump s8 steps from current address in program counter
static void type_jr(cpu_context *ctx) {
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
   u8 c_flag = cpu_flag_C;
   //rotate contents of A register to the right by one
   ctx->registers.a >>= 1;
   //OR contents of A with C flag (shifted to the last bit of A) to update A contents
   ctx->registers.a |=  (c_flag << 7);
   // set C flag to bit value, all other to 0
   cpu_set_flags(0, 0,  0, value);
}
static void type_daa(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_cpl(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_scf(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_ccf(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}

//set CPU halted to true
static void type_halt(cpu_context *ctx) {
  printf("HALT Instruction\n");
  ctx->halted = true;
}
static void type_adc(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_sub(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_sbc(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_and(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}

//OR the contents of two register and save them to register A
static void type_xor(cpu_context *ctx) {
  printf("XOR Instruction\n");
  ctx->registers.a ^= ctx->fetched_data & 0xFF;
  cpu_set_flags(ctx->registers.a == 0,0,0,0);
}
static void type_or(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_cp(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_ret(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_pop(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}

//jump to a certain point in memory if the right flag is set
static void type_jp(cpu_context *ctx) {
  jump_to_address(ctx, ctx->fetched_data, false);
}
static void type_call(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_push(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_rst(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_reti(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}

//disable interrupt master flag
static void type_di(cpu_context *ctx) {
  printf("DI Instruction\n");
  ctx->interrupt_master_enabled_flag = false;
}

//enable interrupt master flag
static void type_ei(cpu_context *ctx) {
  printf("EI Instruction\n");
  ctx->interrupt_master_enabled_flag = true;
}
static void type_ldh(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_jphl(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_cb(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
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


