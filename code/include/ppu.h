#ifndef PPU_H
#define PPU_H

#include <define.h>

typedef enum {
  MODE_HBLANK,
  MODE_VBLANK,
  MODE_OAM_SCAN,
  MODE_DRAW_PIXELS
} ppu_mode;

typedef struct {
  u8 y_position;
  u8 x_position;
  u8 tile_index;
  u8 sprite_attributes;
} sprite;

typedef struct {
  sprite oam[40];
  u8 vram[0x2000];
} ppu_context;


void ppu_init();
void ppu_tick();

u8 ppu_oam_read(u16 address);
void ppu_oam_write(u16 address, u8 value);

u8 ppu_vram_read(u16 address);
void ppu_vram_write(u16 address, u8 value);

#endif
