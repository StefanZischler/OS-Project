#ifndef PF_H
#define PF_H

#include <define.h>
#include <ppu.h>

//used to track current action of the fetcher
typedef enum {
  FETCH_TILE,
  FETCH_DATA_LO,
  FETCH_DATA_HI,
  FETCH_IDLE
} fetch_state;

typedef struct {
  fetch_state state;
  u8 fetcher_x_position;
} fifo_fetcher;

typedef struct fifo_entry{
  struct fifo_entry* next;
  /*u8 color;	//value between 0 and 3
  u8 palette;	//value between 0 and 7
  bool background_priority;*/
  u8 value	//contains color (bits 0,1), palette (bits 2,3,4) & priority (bit 5)
} fifo_entry;

typedef struct {
  fifo_entry* start;
  fifo_entry* end;
  u8 size;	//size between 0 and 16
} fifo_queue;

typedef struct {
  fifo_queue fifo;
  fifo_fetcher fetcher;
  int tick_count;
  
  //track position of fifo
  u8 pushed_x_position;
  
  //the output as list of colors
  u32* video_buffer;
} fifo_context;

fifo_context* fifo_get_context();

void fifo_fetch();

void fifo_process();

#endif
