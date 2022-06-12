#ifndef PF_H
#define PF_H

#include <define.h>
#include <ppu.h>

typedef struct {
  fifo_queue fifo;
  int tick_count;
  
  //the output as list of colors
  u32* video_buffer;
} fifo_context;

fifo_context* fifo_get_context();

void fifo_fetch();

void fifo_process();

#endif
