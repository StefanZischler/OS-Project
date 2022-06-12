#include <stdio.h>
#include <unistd.h>

#include <emulator.h>
#include <cpu.h>
#include <rom.h>
#include <timer.h>
#include <dma.h>

static emu_context ctx;

emu_context *emulator_get_context() {
  return &ctx;
}

int emulator_run(int argc, char ** argv) {
  if(argc < 2) {
    printf("Usage: emulator <rom_file>\n");
    return -1;
  }

  if (!rom_load(argv[1])) {
    printf("Failed to load the ROM file: %s\n", argv[1]);
    return -1;
  }
  
  printf("Cart loaded correctly!\n");

  //initialize CPU
  cpu_init();
  ctx.running = true;
  ctx.paused = false;
  ctx.ticks = 0;

  //start CPU
  while (ctx.running) {
    if (ctx.paused) {
      sleep(10);
    }
    if (!cpu_step()) {
    printf("CPU stopped running!\n");
    return -1;
    }
  }
  
  return 0;
}

void emulator_cycles(int cpu_cycles_number) {
  for(int i=0; i<cpu_cycles_number; i++) {
    //1 cpu cycle == 4 ticks
    for(int j=0; j<4; j++) {
      ctx.ticks++;
      timer_tick();
      //TODO: ppu
    }
    dma_ticks();
  }
}
