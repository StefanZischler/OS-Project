#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include <define.h>


/*
define all the instructions of the instruction set
reference: https://meganesulli.com/generate-gb-opcodes/
*/

/*
Structure of an instruction:
type: What kind of instruction? e.g. ADD or LOAD(LD)
addressing components: What components are being read/written from/to? e.g. reg A and reg HL or reg B and 8-bit data
register type: What register is being read/writtern from/to? e.g. reg A or reg AF
flag type: What flag type is being checked? eg. Flag Z or Flag CY
memory address: In certain instruction e.g. 0xC7 the insructions include a jump to a specific memory address 
*/

//define instruction types
typedef enum {
    //instruction types in the order that they come up in the instruction set
    INS_NONE,
    INS_NOP,
    INS_LD,
    INS_INC,
    INS_DEC,
    INS_RLCA,
    INS_ADD,
    INS_RRCA,
    INS_STOP,
    INS_RLA,
    INS_JR,
    INS_RRA,
    INS_DAA,
    INS_CPL,
    INS_SCF,
    INS_CCF,
    INS_HALT,
    INS_ADC,
    INS_SUB,
    INS_SBC,
    INS_AND,
    INS_XOR,
    INS_OR,
    INS_CP,
    INS_RET,
    INS_POP,
    INS_JP,
    INS_CALL,
    INS_PUSH,
    INS_RST,
    INS_RETI,
    INS_DI,
    INS_EI,
    INS_LDH,
    INS_JPHL,
    INS_CB,
    //CB instruction set
    INS_RLC,
    INS_RRC,
    INS_RL,
    INS_RR,
    INS_SLA,
    INS_SRA,
    INS_SWAP,
    INS_SRL,
    INS_BIT,
    INS_RES,
    INS_SET
} instruction_type;

//addressing components which are involved in the instruction
typedef enum {
    //No register involved
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
    AC_R_A16
} addressing_components;

//define register types
typedef enum {
    REG_NONE,
    //8-bit registers
    REG_A,
    REG_B,
    REG_C,
    REG_D,
    REG_E,
    REG_H,
    REG_L,
    //16-bit registers
    REG_AF,
    REG_BC,
    REG_DE,
    REG_HL,
    //stack pointer
    REG_SP,
    //program counter
    REG_PC,
} register_type;

//define flag types
typedef enum {
    FL_NONE, FL_C, FL_NC, FL_Z, FL_NZ
} flag_type;

//define instruction_set structure
typedef struct {
    instruction_type type;
    addressing_components components;
    register_type register_1;
    register_type register_2;
    flag_type flag;
    u8 memory_address;
} instruction_set;


//get the opcode for the instruction
instruction_set *get_instruction_opcode(u8 opcode);

#endif