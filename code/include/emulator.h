#ifndef EMU_H
#define EMU_H

typedef struct {
  bool paused;
  bool running;
} emu_context;

int emulator_run(int argc, char **argv);

emu_context* emulator_get_context();

#endif
