#ifndef ROM_H
#define ROM_H

// read and write data from the ROM/Cartridge

#include <define.h>

//Cartridge Header - Information on $0100-014F - taken from https://gbdev.io/pandocs/The_Cartridge_Header.html
typedef struct {
    u8 entry_point[4];
    u8 nintendo_logo[0x30];
    char title[16];
    u8 manufacturer_code[4];
    u16 new_licence_code;
    u8 sgb_flag;
    u8 rom_type;
    u8 rom_size;
    u8 ram_size;
    u8 destination_code;
    u8 old_licence_code;
    u8 version_num;
    u8 header_checksum;
    u8 global_checksum;
} rom_header;

//read and write function for the ROM/Cartridge
u8 rom_read(u16 address);
void rom_write(u16 address, u8 value);

//load rom data
bool rom_load(char *rom);

#endif