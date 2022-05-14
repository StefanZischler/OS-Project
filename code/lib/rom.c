#include <rom.h>

//ROM context information
typedef struct {
    char filename[1024];
    u8 *rom_data;
    u32 rom_size;
    rom_header *header;
} rom_context;

static rom_context r_ctx;

//read and write function for the ROM/Cartridge
u8 rom_read(u16 address) {
    //test if Catridge is ROM only (e.g. Tetris)
    if( address < 0x4000) {
        return r_ctx.rom_data[address];
    }
}
void rom_write(u16 address, u8 value) {
    //if ROM only, doesnt need to write to ROM
    return;
}

//load rom data
bool rom_load(char *rom) {
    //TODO: implement load function and display ROM information
}