#ifndef BUS_H
#define BUS_H

// bus for managing messages between emulator parts

#include <define.h>

//read and write function for 16-bit address and 8-bit value
u8 bus_read(u16 address);
void bus_write(u16 address, u8 value);

//read and write function for 16-bit address and 16-bit value
u16 bus_read_16(u16 address);
void bus_write_16(u16 address, u16 value);


#endif