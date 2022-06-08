#ifndef EMU_H
#define EMU_H

#include <stdbool.h>

typedef struct {
  bool paused;
  bool running;
  bool exit;
  int ticks;
} emu_context;

int emulator_run(int argc, char **argv);

void emulator_cycles(int cpu_cycles_number);

emu_context *emulator_get_context();

#endif
