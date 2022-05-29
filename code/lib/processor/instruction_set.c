#include <instruction_set.h>
#include <cpu.h>
#include <bus.h>

instruction_set instructions[0x100] = {

};

instruction_set *get_instruction_opcode(u8 opcode) {
    return &instructions[opcode];
}
