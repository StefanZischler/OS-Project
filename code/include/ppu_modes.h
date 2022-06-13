#ifndef PPU_MODES_H
#define PPU_MODES_H

#include <define.h>

// Control switches between PPU Modes and increase in LY (current line)

// process different modes
void ppu_mode_oam();
void ppu_mode_draw_pixels();
void ppu_mode_hblank();
void ppu_mode_vblank();

//work with LY (current line in LCD)
void ly_next_line();
void ly_load_sprites();

#endif
