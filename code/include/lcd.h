#ifndef LCD_H
#define LCD_H

#include <define.h>

typedef struct {
  u8 lcd_control;
  u8 status_register;
  
  u8 scroll_y;
  u8 scroll_x;
  u8 line_y;
  u8 line_y_compare;
  u8 background_palette;
  u8 object_palette_0;
  u8 object_palette_1;
  u8 window_y;
  u8 window_x;
} lcd_context;

#endif


lcd_context* lcd_get_context();

void lcd_init();
