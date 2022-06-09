#include <ram.h>

/*
C000	CFFF	Work RAM (WRAM)/ROM Bank 0
D000	DFFF	Work RAM (WRAM)/ROM Bank 1-7 - switchable - only for GameBoy Color

FF80	FFFE	High RAM (HRAM)	
*/
typedef struct {
    //define WRAM/HRAM variable size
    u8 work_ram[0x2000];
    u8 high_ram[0x80];
} ram_context;

static ram_context r_ctx;
u8 work_ram_read(u16 address) {
    //normalize address
    address -= 0xC000;
    //return WRAM address
    return r_ctx.work_ram[address];
}
void work_ram_write(u16 address, u8 value) {
    //normalize address
    address -= 0xC000;
    //read value into WRAM address
    r_ctx.work_ram[address] = value;
}

u8 high_ram_read(u16 address) {
    //normalize address
    address -= 0xFF80;
    //return HRAM address
    return r_ctx.high_ram[address];
}

void high_ram_write(u16 address, u8 value) {
    //normalize address
    address -= 0xFF80;
    //read value into HRAM address
    r_ctx.high_ram[address] = value;
}