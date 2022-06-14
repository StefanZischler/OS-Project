#include <ppu.h>
#include <ppu_modes.h>
#include <pixel_fetcher.h>
#include <lcd.h>
#include <string.h>
#include <stdint.h>

//the pixel processing unit is responsible for sprite and tile data

static ppu_context ctx;


ppu_context *ppu_get_context() {
    return &ctx;
}

void ppu_init() {
  
  //PIXEL FIFO
  fifo_get_context()->line_x = 0;
  fifo_get_context()->pushed_x_position = 0;
  fifo_get_context()->fetcher.state = FETCH_TILE;
  fifo_get_context()->fetcher.fetcher_x_position= 0;
  fifo_get_context()->fifo.size = 0;
  fifo_get_context()->fifo.start = fifo_get_context()->fifo.end = NULL;
 
  //OAM Sprites
  ctx.sprite_line = 0;
  ctx.fetched_number_sprites = 0;

  //Window
  ctx.window_current_line = 0;
  //PPU MODES
  ctx.current_display = 0;
  ctx.ticks_on_line = 0;
  printf("allocated %d", X_RESOLUTION * Y_RESOLUTION);
  ctx.display_buffer = malloc(X_RESOLUTION * Y_RESOLUTION * sizeof(32));
  lcd_init();
  //set starting mode of PPU cycle
  PPU_SET_MODE(MODE_OAM_SCAN);
  
  
  //make memory space for OAM and Display 
  memset(ctx.oam, 0, sizeof(ctx.oam));
  memset(ctx.display_buffer, 0, X_RESOLUTION * Y_RESOLUTION * sizeof(u32));

}


//go trough one PPU cycles -> switch between modes
void ppu_tick()  {
  //increase ticks
  ctx.ticks_on_line++;

  //switch between modes 
  switch (PPU_MODES)
  {
  case MODE_OAM_SCAN:
    ppu_mode_oam();
    break;
  case MODE_DRAW_PIXELS:
    ppu_mode_draw_pixels();
    break;
  case MODE_HBLANK:
    ppu_mode_hblank();
    break;
  case MODE_VBLANK:
    ppu_mode_vblank();
    break;
  }

}


u8 ppu_oam_read(u16 address) {
  if(address >= 0xFE00 && address <= 0xFE9F) {	//safety precaution
    //convert address into offset from oam start
    address -= 0xFE00;
    
    /*
    *  convert oam into u8 array
    *  a sprite consists of 4 consecutive array entries
    */
    u8* pointer = (u8*)ctx.oam;
    return pointer[address];
  }
}

void ppu_oam_write(u16 address, u8 value) {
  if(address >= 0xFE00 && address <= 0xFE9F) {	//safety precaution
    //convert address into offset from oam start
    address -= 0xFE00;
    
    /*
    *  convert oam into u8 array
    *  a sprite consists of 4 consecutive array entries
    */
    u8* pointer = (u8*)ctx.oam;
    pointer[address] = value;
  }
}

u8 ppu_vram_read(u16 address) {
  return ctx.vram[address - 0x8000];
}

void ppu_vram_write(u16 address, u8 value) {
  ctx.vram[address - 0x8000] = value;
}
