#ifndef IO_H
#define IO_H

// managing io input and output e.g. joypad, timer, serial, etc. 

#include <define.h>

//read and write function for 16-bit address and 8-bit value
u8 io_read(u16 address);
void io_write(u16 address, u8 value);

#endif