#include <cpu.h>
#include <bus.h>
#include <emulator.h>

/*
This class is for handling everything that has to do with instructions.
This includes fetching data for the instruction in memory or via bus, processing instructions and other 
useful functions to help deal with functions.
*/

/* 
functions that help deal with instructions 
*/

// TODO: implement Stack (for certain instructions necessary)



/* 
functions to fetch data 
*/

// fetch instruction data based on addressing components
// read from bus/registers respectivly and update emu_cycles
void fetch_instruction_data () {

};

// read opcode from bus and get instruction from instruction_set
void fetch_instruction() {

};



/*
functions that process instructions
*/

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
INS_TYPE instruction_get_type (ins_type type ) {
    return type[type];
}