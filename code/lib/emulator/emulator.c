#include <stdio.h>
#include <unistd.h>

#include <emulator.h>
#include <cpu.h>
#include <rom.h>

static emu_context ctx;

emu_context *emulator_get_context() {
  return &ctx;
}

int emulator_run(int argc, char ** argv) {
  if(argc < 2) {
    printf("Usage: emulator <rom_file>\n");
    return -1;
  }
  //TODO: implement rom loading & run cycle

  if (!rom_load(argv[1])) {
    printf("Failed to load the ROM file: %s\n", argv[1]);
    return -1;
  }
  
  printf("Cart loaded correctly!\n");

  cpu_init();

  return 0;
}
