#include <ppu_modes.h>
#include <ppu.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>

extern cpu_context *ctx;

//work with LY (current line in LCD)
void ly_next_line() {
    lcd_get_context()->line_y++;

    //if LY = LY_COMPARE set LYC FLAG
    if (lcd_get_context()->line_y == lcd_get_context()->line_y_compare) {
        LCD_LYC(1);

        //if LCDS in LYC Status request interrupt
        if(LCDS_STATUS_INTERRUPTS(STATUS_LYC)) {
            cpu_request_interrupt(ctx, INTERRUPT_LCD_STAT);
        }
    } else {
        LCD_LYC(0);
    }
}
void ly_load_sprites(){

}

// process different modes

void ppu_mode_oam(){
    if(ppu_get_context()->ticks_on_line >= 80) {
        PPU_SET_MODE(MODE_DRAW_PIXELS);
    }
    //TODO: inizialize fifo pipeline

    //TODO: inizialize sprites
}

void ppu_mode_draw_pixels(){
    if(ppu_get_context()->ticks_on_line >= 80 + 172) {
        PPU_SET_MODE(MODE_HBLANK);
    }
}

void ppu_mode_hblank(){
    if(ppu_get_context()->ticks_on_line >= DOTS_PER_LINE) {
        //end of line -> increase LY
        ly_next_line();
        //end of frame switch to VBlank
        if(lcd_get_context()->line_y >= Y_RESOLUTION) {
            PPU_SET_MODE(MODE_VBLANK);

            cpu_request_interrupt(ctx, INTERRUPT_VBLANK);

            if(LCDS_STATUS_INTERRUPTS(STATUS_VBLANK)) {
                cpu_request_interrupt(ctx, INTERRUPT_LCD_STAT);
            }
        }   
    }
}

void ppu_mode_vblank(){
    if(ppu_get_context()->ticks_on_line >= DOTS_PER_LINE) {
        //end of line -> increase LY
        ly_next_line();
        //end of frame switch to OAM
        if(lcd_get_context()->line_y >= LINES_PER_FRAME) {
            PPU_SET_MODE(MODE_OAM_SCAN);
            //reset ly
            lcd_get_context()->line_y = 0;
        }
        ppu_get_context()->ticks_on_line = 0;
    }
}

