#include <ppu_modes.h>
#include <pixel_fetcher.h>
#include <ppu.h>
#include <lcd.h>
#include <cpu.h>
#include <interrupts.h>
#include <string.h>

extern cpu_context *ctx;


//return if window is visible
bool window_visible() {
    return LDC_WINDOW_ENABLE && lcd_get_context()->window_x >= 0 && 
    lcd_get_context()->window_x <= 166 && lcd_get_context()->window_y >= 0 && 
    lcd_get_context()->window_y <= Y_RESOLUTION; 
}
//work with LY (current line in LCD) and update window
void ly_next_line() {

    //update window line if window visible
    if(window_visible() && lcd_get_context()->line_y >= lcd_get_context()->window_y
    && lcd_get_context()->line_y < lcd_get_context()->window_y + Y_RESOLUTION) {
        ppu_get_context()->window_current_line++;
    }

    //incriment LY
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

//load line of sprites for OAM
void ly_load_sprites(){
    //get current line
    int current_line = lcd_get_context()->line_y;
    //get sprite size
    u8 sprite_size = LDC_OBJ_SIZE;
    //setup memory for sprite array
    memset(ppu_get_context()->sprite_array, 0, sizeof(ppu_get_context()->sprite_array));

    //go trough OAM (40 sprites) and load sprites on current line
    for (int i = 0; i < 40; i++) {
        sprite spt = ppu_get_context()->oam[i]; //grab sprite
        if(!spt.x_position) {
            //if x position is 0 sprite is not visible -> continue
            continue;
        }
        if (ppu_get_context()->sprite_line_number > 10) {
            break; //max of 10 sprite per line
        }
        //test if correct line -> load sprite
        if (spt.y_position <= current_line + 16 && spt.y_position + sprite_size > current_line + 16) {

            sprite_line *spt_line = &ppu_get_context()->sprite_array[ppu_get_context()->sprite_line_number++];
            spt_line->entering = spt;
            spt_line->next = NULL;

            //test if spt_line entry worked if not correct
            if (!ppu_get_context()->sprite_line || ppu_get_context()->sprite_line->entering.x_position > spt.x_position) {
                spt_line->next = ppu_get_context()->sprite_line;
                ppu_get_context()->sprite_line = spt_line;
            }
            //sort the sprite line (sometimes necessary)
            sprite_line *sl = ppu_get_context()->sprite_line;
            sprite_line *prev_sl = sl;

            //go trough sprite line and sort array
            while(sl) {
                if(sl->entering.x_position > spt.x_position) {
                    prev_sl->next = spt_line;
                    spt_line->next = sl;
                    break;
                }
                if(!sl->next) {
                    sl->next = spt_line;
                    break;
                }
                prev_sl = sl;
                sl = sl->next;
            }
        }


    }

}

// process different modes

void ppu_mode_oam(){
    //switch to DRAW PIXELS Mode after 80 dots/ticks
    if(ppu_get_context()->ticks_on_line >= 80) {
        PPU_SET_MODE(MODE_DRAW_PIXELS);
    }
    
    //initialize FIFO variables for next Mode
    fifo_get_context()->pushed_x_position = 0;
    fifo_get_context()->line_x = 0;
    fifo_get_context()->fetcher.state = FETCH_TILE;
    fifo_get_context()->fetcher.fetcher_x_position = 0;
    fifo_get_context()->fifo_position = 0;
    
    //initialize sprite variables for next Mode
    //load sprites at the first OAM tick
    if (ppu_get_context()->ticks_on_line == 1) {
        ppu_get_context()->sprite_line = 0;
        ppu_get_context()->sprite_line_number = 0;
        ly_load_sprites();
    }
}

void ppu_mode_draw_pixels(){
    //start fifo
    fifo_process();
    //switch to next Mode
    if(fifo_get_context()->pushed_x_position >= X_RESOLUTION) {
        fifo_reset(); //reset fifo
        PPU_SET_MODE(MODE_HBLANK); //set HBLANK Mode
        //test if need HBLANK interrupt
        if(LCDS_STATUS_INTERRUPTS(STATUS_HBLANK)) {
                cpu_request_interrupt(ctx, INTERRUPT_LCD_STAT);
            }
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
            ppu_get_context()->current_display++;
        } else {
            PPU_SET_MODE(MODE_OAM_SCAN);
        }   
        ppu_get_context()->ticks_on_line = 0;
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
            ppu_get_context()->window_current_line = 0;
        }
        ppu_get_context()->ticks_on_line = 0;
    }
}

