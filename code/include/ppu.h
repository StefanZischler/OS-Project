#ifndef PPU_H
#define PPU_H

#include <define.h>

typedef struct {
  u8 y_position;
  u8 x_position;
  u8 tile_index;
  u8 sprite_attributes;
} sprite;

typedef struct fifo_entry{
  struct fifo_entry* next;
  u8 color;	//value between 0 and 3
  u8 palette;	//value between 0 and 7
  bool background_priority;
} fifo_entry;

typedef struct {
  fifo_entry* start;
  fifo_entry* end;
  u8 size;	//size between 0 and 16
} fifo_queue;

typedef struct {
  sprite oam[40];
} ppu_context;


void ppu_init();
void ppu_tick();

u8 ppu_oam_read(u16 address);
void ppu_oam_write(u16 address, u8 value);

#endif
