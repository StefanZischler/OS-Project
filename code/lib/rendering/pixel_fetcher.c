#include <pixel_fetcher.h>
#include <ppu.h>
#include <lcd.h>
#include <bus.h>

/*
*  the fifo fetches the data for which color needs to be shown for each pixel
*  on the display.
*  see https://gbdev.io/pandocs/pixel_fifo.html
*/

static fifo_context ctx;


fifo_context* fifo_get_context() {
  return &ctx;
}

void fifo_push(u32 value) {
  fifo_entry entry = { 
    .next = 0,
    .value = value};
  
  if(!ctx.fifo.start) {
    //if the fifo is empty
    ctx.fifo.start = &entry;
  } else {
    //insert the entry at end of queue
    ctx.fifo.end->next = &entry;
    ctx.fifo.end = &entry;
  }
  
  ctx.fifo.size++;
}

bool fifo_add() {
  if(ctx.fifo.size > 8) {
    return false;
    //fifo is full
  }
  
  for(int i=0; i<8; i++) {
    //combine hi and lo into color id
    u8 hilo = (ctx.fetcher.fetch_data_lo & (1 << (7 - i))) >> (7 - i);
    hilo |= ((ctx.fetcher.fetch_data_lo & (1 << (7 - i))) >> (7 - i)) << 1;
    
    u32 color = lcd_get_context()->background_colors[hilo];
    
    //TODO: check cases
    
    //check if pixel is on screen
    //screen starts usually at x==8 but can be scrolled
    if((ctx.fetcher.fetcher_x_position * 8) - (8 - (lcd_get_context()->scroll_x % 8))) {
      fifo_push(color);
    }
  }
  return true;
}

void fifo_fetch() {
  //TODO: implement function
  switch(ctx.fetcher.state) {
    case FETCH_TILE:
      if(LDC_PRIORITY_BG_WINDOW) {
        //is background enabled
        
        //get the background tile
        //address calculation from https://gbdev.io/pandocs/pixel_fifo.html#get-tile
        ctx.fetcher.fetched_tile = bus_read(LDC_BG_WINDOW_TILE +
         (((lcd_get_context()->scroll_x / 8) + ctx.fetcher.fetcher_x_position) & 0x1F) +
         ((lcd_get_context()->line_y + lcd_get_context()->scroll_y) & 255));
         
         //TODO: increment tile id?
      }
      
      //TODO: get sprite data
      
      ctx.fetcher.state = FETCH_DATA_LO;
      ctx.fetcher.fetcher_x_position++;
      break;
    case FETCH_DATA_LO:
      //gets lower parts of color id for a line of 8 pixels
      //information on how pixel data is stored taken from https://gbdev.io/pandocs/Tile_Data.html
      ctx.fetcher.fetch_data_lo = bus_read(LDC_BG_WINDOW_TILE +
        (ctx.fetcher.fetched_tile * 16) +
        ctx.fetcher.tile_y_position);
        
        //TODO: load sprite data
      ctx.fetcher.state = FETCH_DATA_HI;
      break;
    case FETCH_DATA_HI:
      //same as FETCH_DATA_LO but incremented by 1 to get higher part of color id
      ctx.fetcher.fetch_data_lo = bus_read(LDC_BG_WINDOW_TILE +
        (ctx.fetcher.fetched_tile * 16) +
        ctx.fetcher.tile_y_position + 1);
        
        //TODO: load sprite data
      ctx.fetcher.state = FETCH_DATA_LO;
      break;
    case FETCH_IDLE:
      ctx.fetcher.state = FETCH_PUSH;
      break;
    case FETCH_PUSH:
      //keep pushing until it succeeds
      if(fifo_add()) {
        ctx.fetcher.state = FETCH_TILE;
      }
      break;
  }
}

void fifo_push_pixel() {
  //TODO: implement function
}

void fifo_process() {
  ctx.tick_count++;
  
  
  if(ctx.tick_count & 1) {
    fifo_fetch();
  }
  
  fifo_push_pixel;
}
