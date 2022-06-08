#include <cpu.h>
#include <bus.h>
#include <emulator.h>
#include <instruction_handling.h>

cpu_context ctx = {0};

/*
This class is for handling everything that has to do with instructions.
This includes fetching data for the instruction in memory or via bus, processing instructions and other 
useful functions to help deal with functions.
*/

/* 
functions that help deal with instructions 
*/

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

// TODO: implement Stack (for certain instructions necessary)



/* 
functions to fetch data 
*/

// fetch instruction data based on addressing components
// read from bus/registers respectivly and update emu_cycles
void fetch_instruction_data () {
    ctx.current_opcode = bus_read(ctx.registers.pc++);
    ctx.current_instruction = get_instruction_opcode(ctx.current_opcode);
};

static void type_none(cpu_context *ctx) {
  printf("Not a real instruction...\n");
  exit(-2);
}
static void type_nop(cpu_context *ctx) {
  //This function should do nothing
  printf("NOP Instruction\n");
}
static void type_ld(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_inc(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_dec(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_rlc(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_add(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_rrc(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_stop(cpu_context *ctx) {
  fprintf(stderr, "Stopping CPU!\n");
}
static void type_rla(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_jr(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_rra(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
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
static void type_halt(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
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
static void type_jp(cpu_context *ctx) {
  printf("Jump Instruction\n");
  if(check_flag(ctx)) {
    ctx->registers.pc = ctx->fetched_data;
    emulator_cycles(1);
  }
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
static void type_di(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
}
static void type_ei(cpu_context *ctx) {
  printf("Instruction %02X not implemented yet...\n", ctx->current_opcode);
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
    [INS_RLC] = type_rlc,
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



// read opcode from bus and get instruction from instruction_set
void fetch_instruction() {
    ctx.memory_destination = 0;
    ctx.destination_is_in_memory = false;

    // test if there is current istruction
    // return 0 if not
    if(ctx.current_instruction == NULL) {
        return;
    }

    //switch between all addressing component cases
    switch (ctx.current_instruction->components) {
   /*
    AC_NONE,
    //registers involved 
    //save 16-bit data to register
    AC_R_D16,
    //save data from one register in another register
    AC_R_R,
    //store data from one register into memory location specified by another register
    AC_MR_R,
    //change contents of register
    AC_R,
    //save 8-bit data to register
    AC_R_D8,
    //load data to one register into memory location specified by another register
    AC_R_MR,
    //load data from a memory location specified by HL into another register and increment contents of HL
    AC_R_HLI,
    //load data from a memory location specified by HL into another register and decrease contents of HL
    AC_R_HLD,
    //save data from a register to a memory location specified by HL and increment contents of HL
    AC_HLI_R,
    //save data from a register to a memory location specified by HL and decrease contents of HL
    AC_HLD_R,
    //load data of internal RAM, port register or mode register at address range 0xFF00-0xFFFF specified by 8-bit operand a8 to a register
    AC_R_A8,
    //save data of internal RAM, port register or mode register at address range 0xFF00-0xFFFF specified by 8-bit operand a8 to a register
    AC_A8_R,
    //add 8-bit operand SPR to stack pointer and store result in HL
    AC_HL_SPR,
    //do a logical operation e.g. OR, AND, etc. with contents of d8 on register A
    AC_D8,
    //work with a 16-bit value; used to set flags
    AC_D16,
    //save data from 8-bit operand d8 to a memory location specified by HL 
    AC_MR_D8,
    //make operations on memory register HL
    AC_MR,
    //save contents of a register in internal RAM or register specified by a16
    AC_A16_R,
    //depending on which flag is set load a16 into the program counter or follow the subsequent instruction starting a address a16
    AC_R_A16*/
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

    //TODO: rest of addressing components modes
    case AC_R_MR:
        return;
    case AC_R_HLI:
        return;
    case AC_R_HLD:
        return; 
    case AC_HLI_R:
        return;
    case AC_HLD_R:
        return;
    case AC_R_A8:
        return; 
    case AC_A8_R:
        return;
    case AC_HL_SPR:
        return;
    case AC_D8:
        return; 
    case AC_MR_D8:
        return;
    case AC_MR:
        return;
    case AC_A16_R:
        return; 
    case AC_R_A16:
        return;     
    
    default:
        printf("Unknown Addressing Components! %d\n", ctx.current_instruction->components);
        return;
    }
};