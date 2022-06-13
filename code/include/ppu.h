#ifndef PPU_H
#define PPU_H

#include <define.h>

typedef struct {
  u8 y_position;
  u8 x_position;
  u8 tile_index;
  u8 sprite_attributes;
} sprite;

typedef struct {
  sprite oam[40];
  u8 vram[0x2000];
  u8 mode;
  u32 ticks_on_line;
  u32 *display_buffer;
} ppu_context;


typedef enum {
  MODE_HBLANK,
  MODE_VBLANK,
  MODE_OAM_SCAN,
  MODE_DRAW_PIXELS
} ppu_mode;

ppu_context *ppu_get_context();
//define for easy access to PPU modes (first bit determines mode)
#define PPU_MODES ((ppu_mode)(ppu_get_context()->mode & 0b11))
#define PPU_SET_MODE(set_mode) {ppu_get_context()->mode &= ~0b11; ppu_get_context()->mode |= set_mode;}

//define static variables for LCD 
static const int DOTS_PER_LINE = 456;
static const int LINES_PER_FRAME = 154;
static const int X_RESOLUTION = 160;
static const int Y_RESOLUTION = 140;

void ppu_init();
//go trough one PPU cycles -> switch between modes
void ppu_tick();

u8 ppu_oam_read(u16 address);
void ppu_oam_write(u16 address, u8 value);

u8 ppu_vram_read(u16 address);
void ppu_vram_write(u16 address, u8 value);

#endif
