#ifndef PF_H
#define PF_H

#include <define.h>
#include <ppu.h>

typedef struct fifo_entry{
  struct fifo_entry* next;
  u8 color;	//value between 0 and 3
  u8 palette;	//value between 0 and 7
  bool background_priority;
} fifo_entry;

typedef struct {
  fifo_entry* start;
  fifo_entry* end;
  u8 size;	//size between 0 and 16
} fifo_queue;

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
