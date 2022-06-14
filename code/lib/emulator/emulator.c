#include <stdio.h>
#include <unistd.h>

#include <emulator.h>
#include <cpu.h>
#include <rom.h>
#include <timer.h>
#include <dma.h>
#include <ppu.h>
#include <game_window.h>

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

  //intialize game window
  render_game_windnow();
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

  //update window
  u32 previous_display = 0;
  while(!ctx.exit) {
    usleep(1000);
    handel_events();
    if (previous_display != ppu_get_context()->current_display) {
      update_window();
    }
    previous_display = ppu_get_context()->current_display;
  }
  
  return 0;
}

void emulator_cycles(int cpu_cycles_number) {
  for(int i=0; i<cpu_cycles_number; i++) {
    //1 cpu cycle == 4 ticks
    for(int j=0; j<4; j++) {
      ctx.ticks++;
      timer_tick();
      ppu_tick();
    }
    dma_ticks();
  }
}
