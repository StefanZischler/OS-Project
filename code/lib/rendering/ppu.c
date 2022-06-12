#include <ppu.h>

//the pixel processing unit is responsible for sprite and tile data

static ppu_context ctx;


u8 ppu_oam_read(u16 address) {
  if(address >= 0xFE00 && address <= 0xFE9F) {	//safety precaution
    //convert address into offset from oam start
    address -= 0xFE00;
    
    /*
    *  convert oam into u8 array
    *  a sprite consists of 4 consecutive array entries
    */
    u8* pointer = (u8*)ctx.oam;
    return pointer[address];
  }
}

void ppu_oam_write(u16 address, u8 value) {
  if(address >= 0xFE00 && address <= 0xFE9F) {	//safety precaution
    //convert address into offset from oam start
    address -= 0xFE00;
    
    /*
    *  convert oam into u8 array
    *  a sprite consists of 4 consecutive array entries
    */
    u8* pointer = (u8*)ctx.oam;
    pointer[address] = value;
  }
}

u8 ppu_vram_read(u16 address) {
  return ctx.vram[address];
}

void ppu_vram_write(u16 address, u8 value) {
  ctx.vram[address] = value;
}
