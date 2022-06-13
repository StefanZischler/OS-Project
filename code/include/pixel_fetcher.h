#ifndef PF_H
#define PF_H

#include <define.h>
#include <ppu.h>

//used to track current action of the fetcher
typedef enum {
  FETCH_TILE,
  FETCH_DATA_LO,
  FETCH_DATA_HI,
  FETCH_IDLE,
  FETCH_PUSH
} fetch_state;

typedef struct {
  fetch_state state;
  
  //positions & coordinates
  u8 fetcher_x_position;
  u8 tile_y_position;
  
  //tile data
  u8 fetched_tile;
  u8 fetch_data_lo;
  u8 fetch_data_hi;
  u8 fetched_sprite_data[20];	//lo/hi alternating
} fifo_fetcher;

typedef struct fifo_entry{
  struct fifo_entry* next;
  /*u8 color;	//value between 0 and 3
  u8 palette;	//value between 0 and 7
  bool background_priority;*/
  u32 value;	//contains color for display
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
  u8 line_x;
  u8 tilemap_x;
  u8 tilemap_y;
  
  //the output as list of colors
  u32* video_buffer;
} fifo_context;

fifo_context* fifo_get_context();

void fifo_process();
void fifo_reset();

#endif
