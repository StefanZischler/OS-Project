#ifndef RAM_H
#define RAM_H

// read and write data from the Work RAM (WRAM)/ROM Bank 0 
// and High RAM (HRAM)	

#include <define.h>

//read and write function for WRAM and HRAM
u8 work_ram_read(u16 address);
void work_ram_write(u16 address, u8 value);

u8 high_ram_read(u16 address);
void high_ram_write(u16 address, u8 value);

#endif