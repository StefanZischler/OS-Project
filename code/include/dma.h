#ifndef DMA_H
#define DMA_H

#include <define.h>

/*
DMA is responsible for writing from ROM/RAM to OAM(Object Attribute Memory).
So it handles communication between CPU and Video RAM/OAM.
Has to take into consideration which mode PPU is in. Only transfer data if NOT in OAM Search and Pixel transferring (MODE 2/3) in.
*/

//bool for testing if DMA is currently transferring data
bool is_dma_transferring();

//test if DMA is active and when ready start OAM write from PPU
void dma_ticks();

//set beginning variables for DMA
void dma_beginning(u8 start_value);


#endif