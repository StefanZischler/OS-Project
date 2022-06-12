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
