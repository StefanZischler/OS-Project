#include <lcd.h>
#include <dma.h>

static lcd_context ctx;

lcd_context* lcd_get_context() {
  return &ctx;
}

//inizialize standart color palette of GameBoy (hexcolors)
//white = 0xFFFFFFFF (#FFFFFF), grey = 0xFFAAAAAA (#AAAAAA), 
//dark grey = 0xFF555555 (#555555), black = 0xFF000000 (#000000)
static unsigned long standart_colors[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
//initialization, values taken from
//gbdev.io/pandocs/Power_Up_Sequence.html#hardware-registers
void lcd_init() {
  //register
  ctx.lcd_control = 0x91;
  ctx.status_register = 0x81;	
  //draw pixel line information
  ctx.scroll_y = 0x00;
  ctx.scroll_x = 0x00;
  ctx.line_y = 0x91;
  ctx.line_y_compare = 0x00;
  //color palettes
  ctx.background_palette = 0xFC;
  ctx.object_palette_0 = 0xFF;	
  ctx.object_palette_1 = 0xFF;
  //window
  ctx.window_y = 0x00;
  ctx.window_x = 0x00;
  // inizilite pixel colors (4 grey tones)
  for (int i = 0; i < 4; i++) {
    ctx.background_colors[i] = standart_colors[i];
    ctx.sprite_1_color[i] = standart_colors[i];
    ctx.sprite_2_color[i] = standart_colors[i];
  }
}

u8 lcd_read(u16 address) {
  switch(address) {
    case 0xFF40:
      return ctx.lcd_control;
    case 0xFF41:
      return ctx.status_register;
    case 0xFF42:
      return ctx.scroll_y;
    case 0xFF43:
      return ctx.scroll_x;
    case 0xFF44:
      return ctx.line_y;
    case 0xFF45:
      return ctx.line_y_compare;
    case 0xFF46:
      //DMA only needs write
      break;
    case 0xFF47:
      return ctx.background_palette;
    case 0xFF48:
      return ctx.object_palette_0;
    case 0xFF49:
      return ctx.object_palette_1;
    case 0xFF4A:
      return ctx.window_y;
    case 0xFF4B:
      return ctx.window_x;
  }
}

//update the color palletes
void update_color_palette(u8 palette_colors, u8 palette) {
  //get backround palette colors
  u32 *colors = ctx.background_colors;

  //switch depending which palette used
  switch (palette)
  {
  case 1:
    colors = ctx.sprite_1_color;
    break;
  
  case 2:
  colors = ctx.sprite_2_color;
    break;
  }

  //update palette (at the right bit)
  colors[0] = standart_colors[palette_colors & 0b11];
  colors[1] = standart_colors[(palette_colors >> 2) & 0b11];
  colors[2] = standart_colors[(palette_colors >> 4) & 0b11];
  colors[3] = standart_colors[(palette_colors >> 6) & 0b11];
}

void lcd_write(u16 address, u8 value) {
  switch(address) {
    case 0xFF40:
      ctx.lcd_control = value;
      break;
    case 0xFF41:
      ctx.status_register = value;
      break;
    case 0xFF42:
      ctx.scroll_y = value;
      break;
    case 0xFF43:
      ctx.scroll_x = value;
      break;
    case 0xFF44:
      ctx.line_y = value;
      break;
    case 0xFF45:
      ctx.line_y_compare = value;
      break;
    case 0xFF46:
      dma_beginning(value);
    case 0xFF47:
    //update backround color palette
      update_color_palette(value, 0);
      break;
    case 0xFF48:
    //update color palette (besides last two bit, since they are transparent)
      update_color_palette(value & 0b11111100, 1);
      break;
    case 0xFF49:
    //update color palette (besides last two bit, since they are transparent)
      update_color_palette(value & 0b11111100, 2);
      break;
    case 0xFF4A:
      ctx.window_y = value;
      break;
    case 0xFF4B:
      ctx.window_x = value;
      break;
  }
}
