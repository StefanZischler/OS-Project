#include <pixel_fetcher.h>
#include <ppu.h>

static fifo_context ctx;


fifo_context* fifo_get_context() {
  return &ctx;
}

void fifo_fetch() {
  //TODO: implement function
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
