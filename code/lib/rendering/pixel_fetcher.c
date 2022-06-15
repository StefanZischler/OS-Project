#include <stdio.h>

#include <pixel_fetcher.h>
#include <ppu.h>
#include <lcd.h>
#include <bus.h>

/*
*  the fifo fetches the data for which color needs to be shown for each pixel
*  on the display.
*  see https://gbdev.io/pandocs/pixel_fifo.html
*
*  the Gameboy has 2 fifo-queues - one for the background/window and one for the sprites
*  the queues are always popped at the same time and one of the results is discarded
*  this implementation combines the fifo-queues and only saves the pixel color
*  that will be returned at pop()
*/

static fifo_context ctx;


fifo_context* fifo_get_context() {
  return &ctx;
}

void fifo_push(u32 value) {
  fifo_entry *entry = malloc(sizeof(fifo_entry));
  entry->next = NULL;
  entry->value = value;
  
  if(!ctx.fifo.start) {
    //if the fifo is empty
    ctx.fifo.start = entry;
    ctx.fifo.end = entry;
  } else {
    //insert the entry at end of queue
    ctx.fifo.end->next = entry;
    ctx.fifo.end = entry;
  }
  
  ctx.fifo.size++;
}

u32 fifo_pop() {
  if(ctx.fifo.size > 0) {
    fifo_entry* entry = ctx.fifo.start;
    ctx.fifo.start = entry->next;
    ctx.fifo.size--;
    u32 result = entry->value;
    free(entry);
    return result;
  }
}

u32 fifo_fetch_sprite_color(u8 background_color);
//return if window is visible
bool window_visible_fetcher() {
    return LDC_WINDOW_ENABLE && lcd_get_context()->window_x >= 0 && 
    lcd_get_context()->window_x <= 166 && lcd_get_context()->window_y >= 0 && 
    lcd_get_context()->window_y <= Y_RESOLUTION; 
}

//load sprite tile 
void fifo_load_sprite_tile() {
  //load sprite array
  sprite_line *sl = ppu_get_context()->sprite_line;
  //loop through sprite array 
  while(sl) {
    //get next sprite
    int sprite_x = (sl->entering.x_position - 8) + (lcd_get_context()->scroll_x % 8);
    //test if sprite is right size for data processing
    if ((sprite_x >= ctx.fetcher.fetcher_x_position && sprite_x < ctx.fetcher.fetcher_x_position + 8) ||
    ((sprite_x + 8) >= ctx.fetcher.fetcher_x_position && (sprite_x + 8) < ctx.fetcher.fetcher_x_position + 8)) {
      //add new sprite to fetched sprites array
      ppu_get_context()->fetched_sprites[ppu_get_context()->fetched_number_sprites++];
    }
    //go to next sprite
    sl = sl->next;
    //test if sprite exceeded array size of 3
    if(!sl || ppu_get_context()->fetched_number_sprites >= 3) {
      break;
    }
  }
  
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
    if(!LDC_PRIORITY_BG_WINDOW) {
      //background is not enabled
      //color = lcd_get_context()->background_colors[0];
      hilo = 0;
    }
    if(LDC_OBJ_ENABLE) {
      //non-transparent pixels of sprites replace background pixels
      color = fifo_fetch_sprite_color(hilo);
    }
    
    //check if pixel is on screen
    //screen starts usually at x==8 but can be scrolled
    if((ctx.fetcher.fetcher_x_position * 8) - (8 - (lcd_get_context()->scroll_x % 8))) {
      fifo_push(color);
      ctx.fifo_position++;
    }
  }
  return true;
}

void fifo_load_window() {
  if (!window_visible_fetcher()) {
    return;
  }
//temp variable for convenience
    u8 y = lcd_get_context()->window_y;
  //don't load window if it is not visible
  //visibility depends on the window enable bit of the lcdc and the window position
  if(!(LDC_WINDOW_ENABLE && y >= 0 && y <= 143 && lcd_get_context()->window_x >= 0 && lcd_get_context()->window_x <= 166)) {
    return;
  }
  //WY condition
  if(lcd_get_context()->line_y < y || lcd_get_context()->line_y >= y + 160) {
    return;
  }
  //WX condition
  if(ctx.fetcher.fetcher_x_position * 8 + 7 < lcd_get_context()->window_x || ctx.fetcher.fetcher_x_position * 8 + 7 >= lcd_get_context()->window_x + 143 + 14) {
    return;
  }
  
  //divide to get y position for tile index
  u8 tile_y = ppu_get_context()->window_current_line / 8;
  ctx.fetcher.fetched_tile = bus_read(LDC_TILE_MAP + 
    ((ctx.fetcher.fetcher_x_position + 7 - lcd_get_context()->window_x) / 8) +
    (tile_y * 32));
    
    if(LDC_BG_WINDOW_TILE == 0x8800) {
      //in this address mode instead of 0x8000 + 0-255
      //the address is 0x9000 + (-128)-127 
      ctx.fetcher.fetched_tile += 128;
    }
}

u32 fifo_fetch_sprite_color(u8 background_color) {
  u32 color = lcd_get_context()->background_colors[background_color];
  u8 previous_sprite_x = 167;
  //iterate over all (shown) sprites on the current line
  for(int i=0; i<ppu_get_context()->fetched_number_sprites; i++) {
    //get position of sprite on screen
    u8 sprite_x = ppu_get_context()->fetched_sprites[i].x_position - 8 + (lcd_get_context()->scroll_x % 8);
    
    //check if sprite overlaps with fetcher position
    if(sprite_x + 8 < ctx.fifo_position || sprite_x > ctx.fifo_position) {
      continue;
    }
    
    
    int offset = ctx.fifo_position - sprite_x;
    if(ppu_get_context()->fetched_sprites[i].sprite_attributes & (1 << 5)) {
      //sprite is flipped on x-axis
      offset = 7 - offset;
    }
    
    u8 lo = (ctx.fetcher.fetched_sprite_data[i * 2] & (1 << (7 - offset))) >> (7 - offset);
    u8 hi = ((ctx.fetcher.fetched_sprite_data[(i * 2) + 1] & (1 << (7 - offset))) >> (7 - offset)) << 1;
    
    //check if pixel is transparent
    if(!(hi | lo)) {
      continue;
    }
    
    //check drawing priority
    //sprite with lowest x-position has highest priority
    if(sprite_x < previous_sprite_x) {
      previous_sprite_x = sprite_x;
      
      bool bg_over_obj = ppu_get_context()->fetched_sprites[i].sprite_attributes & (1 << 7);
      if(background_color == 0 || !bg_over_obj) {
        //determine the palette used
        if(ppu_get_context()->fetched_sprites[i].sprite_attributes & (1 << 4)) {
          color = lcd_get_context()->sprite_2_color[hi | lo];
        } else {
          color = lcd_get_context()->sprite_1_color[hi | lo];
        }
      }
    }
  }
  
  return color;
}

//offset should be 0 for lo bits or 1 for hi bits
void fifo_load_sprites(u8 offset) {
  //sprites can be either 8 or 16 pixels high
  u8 sprite_height = LDC_OBJ_SIZE;
  
  //iterate over all (shown) sprites on the current line
  for(int i=0; i<ppu_get_context()->fetched_number_sprites; i++) {
    //determine which line of the sprite must be loaded
    //the sprite y-pos is 16 higher than the line count because sprites can
    //go 16 pixels above the screen (so only part of the sprite is visible)
    u8 tile_line = (lcd_get_context()->line_y + 16) - ppu_get_context()->fetched_sprites[i].y_position;
    
    //check if the sprite is flipped on the y-axis
    if(ppu_get_context()->fetched_sprites[i].sprite_attributes & (1 << 6)) {
      tile_line = sprite_height - 1 - tile_line;
    }
    
    //in 16x8 mode the least significant bit of the index is ignored
    //see https://gbdev.io/pandocs/OAM.html#byte-2---tile-index
    u8 tile_index = ppu_get_context()->fetched_sprites[i].tile_index;
    if(sprite_height == 16) {
      tile_index = tile_index & ~(1);
    }
    
    ctx.fetcher.fetched_sprite_data[(i * 2) + offset] = bus_read(0x8000 + (tile_index * 16) + (tile_line * 2) + offset);
  }
}

void fifo_fetch() {
  switch(ctx.fetcher.state) {
    case FETCH_TILE:
      if(LDC_PRIORITY_BG_WINDOW) {
        //is background enabled
        ppu_get_context()->fetched_number_sprites = 0;
        //get the background tile
        //address calculation from https://gbdev.io/pandocs/pixel_fifo.html#get-tile
        ctx.fetcher.fetched_tile = bus_read(LDC_BG_WINDOW_TILE +
         (((lcd_get_context()->scroll_x / 8) + ctx.fetcher.fetcher_x_position) & 0x1F) +
         ((lcd_get_context()->line_y + lcd_get_context()->scroll_y) & 255));
         
         if(LDC_BG_WINDOW_TILE == 0x8800) {
           //in this address mode instead of 0x8000 + 0-255
           //the address is 0x9000 + (-128)-127 
           ctx.fetcher.fetched_tile += 128;
         }
         //if the window is visible on the current position, the window tile overwrites
        //the background tile
        fifo_load_window();

      }

      //load sprite tiles
      if (LDC_OBJ_ENABLE && ppu_get_context()->sprite_line) {
        fifo_load_sprite_tile();
      }  
      
      ctx.fetcher.state = FETCH_DATA_LO;
      ctx.fetcher.fetcher_x_position++;
      break;
    case FETCH_DATA_LO:
      //gets lower parts of color id for a line of 8 pixels
      //information on how pixel data is stored taken from https://gbdev.io/pandocs/Tile_Data.html
      ctx.fetcher.fetch_data_lo = bus_read(LDC_BG_WINDOW_TILE +
        (ctx.fetcher.fetched_tile * 16) +
        ctx.fetcher.tile_y_position);
        
        //gets sprite information for lo
        fifo_load_sprites(0);
      ctx.fetcher.state = FETCH_DATA_HI;
      break;
    case FETCH_DATA_HI:
      //same as FETCH_DATA_LO but incremented by 1 to get higher part of color id
      ctx.fetcher.fetch_data_lo = bus_read(LDC_BG_WINDOW_TILE +
        (ctx.fetcher.fetched_tile * 16) +
        ctx.fetcher.tile_y_position + 1);
        
        //gets sprite information for hi
        fifo_load_sprites(1);
      ctx.fetcher.state = FETCH_IDLE;
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
  if(ctx.fifo.size <= 8) {
    //fifo may not go below 8 entries in queue
    return;
  }
  
  if(ctx.line_x >= lcd_get_context()->scroll_x % 8) {
    ppu_get_context()->display_buffer[ctx.pushed_x_position + lcd_get_context()->line_y * 160] = fifo_pop();
    ctx.pushed_x_position++;
  }
  
  ctx.line_x++;
}

void fifo_process() {
  ctx.fetcher.tile_y_position = ((lcd_get_context()->line_y + lcd_get_context()->scroll_y) % 8) *2;
  ctx.tilemap_x = (ctx.fetcher.fetcher_x_position + lcd_get_context()->scroll_x);
  ctx.tilemap_y = (lcd_get_context()->line_y + lcd_get_context()->scroll_y);

  if(!(ppu_get_context()->ticks_on_line & 1)) {
    fifo_fetch();
  }
  
  fifo_push_pixel();
}

void fifo_reset() {
  while(ctx.fifo.size > 0) {
    fifo_pop();
  }
  ctx.fifo.start = 0;
}
