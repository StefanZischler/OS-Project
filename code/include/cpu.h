#ifndef CPU_H
#define CPU_H

#include <define.h>

typedef struct {
  u8 a;
  u8 b;
  u8 c;
  u8 d;
  u8 e;
  u8 f;
  u8 h;
  u8 l;
  u16 sp;
  u16 pc;
} cpu_registers;

typedef struct {
  cpu_registers registers;
} cpu_context;

bool cpu_flag_Z();
bool cpu_flag_N();
bool cpu_flag_H();
bool cpu_flag_C();

void cpu_set_flags(bool z, bool n, bool h, bool c);
void cpu_set_flag(bool n, int offset);

void cpu_init();

#endif
