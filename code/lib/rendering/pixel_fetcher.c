#include <pixel_fetcher.h>
#include <ppu.h>

/*
*  the fifo fetches the data for which color needs to be shown for each pixel
*  on the display.
*  see https://gbdev.io/pandocs/pixel_fifo.html
*/

static fifo_context ctx;


fifo_context* fifo_get_context() {
  return &ctx;
}

void fifo_push(u8 value) {
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

void fifo_fetch() {
  //TODO: implement function
  switch(ctx.fetcher.state) {
    case FETCH_TILE:
      ctx.fetcher.state = FETCH_DATA_LO;
      break;
    case FETCH_DATA_LO:
      ctx.fetcher.state = FETCH_DATA_HI;
      break;
    case FETCH_DATA_HI:
      ctx.fetcher.state = FETCH_DATA_LO;
      break;
    case FETCH_IDLE:
      ctx.fetcher.state = FETCH_TILE;
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
