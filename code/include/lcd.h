#ifndef LCD_H
#define LCD_H

#include <define.h>

/*
LCD Display: Manage display, pixel colors, windows, status, etc.

*/
typedef struct {
  //registers
  u8 lcd_control;
  u8 status_register;
  //draw pixel line information
  u8 scroll_y;
  u8 scroll_x;
  u8 line_y;
  u8 line_y_compare;
  //color palettes
  u8 background_palette;
  u8 object_palette_0;
  u8 object_palette_1;
  //window
  u8 window_y;
  u8 window_x;
  u8 window_line;
  //pixel colors (4 grey tones)
  u32 background_colors[4];
  u32 sprite_1_color[4];
  u32 sprite_2_color[4];
} lcd_context;

lcd_context* lcd_get_context();


/* 
FF40 LCD Control (LDLC) https://gbdev.io/pandocs/LCDC.html
7	LCD and PPU enable	0=Off, 1=On
6	Window tile map area	0=9800-9BFF, 1=9C00-9FFF
5	Window enable	0=Off, 1=On
4	BG and Window tile data area	0=8800-97FF, 1=8000-8FFF
3	BG tile map area	0=9800-9BFF, 1=9C00-9FFF
2	OBJ size	0=8x8, 1=8x16
1	OBJ enable	0=Off, 1=On
0	BG and Window enable/priority	0=Off, 1=On
*/

//helper define to get right bit for LCDC 
#define FIND_BIT(target, offset) ((target & (1 << offset)) ? 1 : 0)
#define LDC_PPU_ENABLE (FIND_BIT(lcd_get_context()->lcd_control, 7))
#define LDC_TILE_MAP (FIND_BIT(lcd_get_context()->lcd_control, 6) ? 0x9C00 : 0x9800)
#define LDC_WINDOW_ENABLE (FIND_BIT(lcd_get_context()->lcd_control, 5))
#define LDC_BG_WINDOW_TILE (FIND_BIT(lcd_get_context()->lcd_control, 4) ? 0x8000 : 0x8800)
#define LDC_TILE_AREA (FIND_BIT(lcd_get_context()->lcd_control, 3) ? 0x9C00 : 0x9800)
#define LDC_OBJ_SIZE (FIND_BIT(lcd_get_context()->lcd_control, 2) ? 16 : 8)
#define LDC_OBJ_ENABLE (FIND_BIT(lcd_get_context()->lcd_control, 1))
#define LDC_PRIORITY_BG_WINDOW (FIND_BIT(lcd_get_context()->lcd_control, 0))

/*
FF41 - STAT (LCD Status) (R/W) (https://gbdev.io/pandocs/STAT.html)
Bit 6 - LYC=LY STAT Interrupt source         (1=Enable) (Read/Write)
Bit 5 - Mode 2 OAM STAT Interrupt source     (1=Enable) (Read/Write)
Bit 4 - Mode 1 VBlank STAT Interrupt source  (1=Enable) (Read/Write)
Bit 3 - Mode 0 HBlank STAT Interrupt source  (1=Enable) (Read/Write)
Bit 2 - LYC=LY Flag                          (0=Different, 1=Equal) (Read Only)
Bit 1-0 - Mode Flag                          (Mode 0-3, see below) (Read Only)
          0: HBlank
          1: VBlank
          2: Searching OAM
          3: Transferring Data to LCD Controller
*/ 
typedef enum {
  STATUS_LYC = (1 << 6),
  STATUS_OAM = (1 << 5),
  STATUS_VBLANK = (1 << 4),
  STATUS_HBLANK = (1 << 3)
} lcd_status;

#define LCDS_STATUS_INTERRUPTS(interrupt) (lcd_get_context()->status_register & interrupt)

//helper function to set bit for LYC
#define SET_BIT(target, offset, bit) {if (bit) target |= (1 << offset); else target &= ~(1 << offset);}
#define LCD_LYC(bit) (SET_BIT(lcd_get_context()->status_register, 2, bit))


void lcd_init();

u8 lcd_read(u16 address);
void lcd_write(u16 address, u8 value);


#endif

