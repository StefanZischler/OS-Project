#include <lcd.h>

static lcd_context ctx;

lcd_context* lcd_get_context() {
  return &ctx;
}

//initialization, values taken from
//gbdev.io/pandocs/Power_Up_Sequence.html#hardware-registers
void lcd_init() {
  ctx.lcd_control = 0x91;
  ctx.status_register = 0x81;	//TODO: which bootloader to use
  ctx.scroll_y = 0x00;
  ctx.scroll_x = 0x00;
  ctx.line_y = 0x91;
  ctx.line_y_compare = 0x00;
  ctx.background_palette = 0xFC;
  ctx.object_palette_0 = 0x00;	//actual start value unknown
  ctx.object_palette_1 = 0x00;
  ctx.window_y = 0x00;
  ctx.window_x = 0x00;
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
      //TODO: dma
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
      //TODO: dma
      break;
    case 0xFF47:
      ctx.background_palette = value;
      break;
    case 0xFF48:
      ctx.object_palette_0 = value;
      break;
    case 0xFF49:
      ctx.object_palette_1 = value;
      break;
    case 0xFF4A:
      ctx.window_y = value;
      break;
    case 0xFF4B:
      ctx.window_x = value;
      break;
  }
}
