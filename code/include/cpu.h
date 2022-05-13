#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t f;
  uint8_t h;
  uint8_t l;
  uint16_t sp;
  uint16_t pc;
} cpu_registers;

typedef struct {
  cpu_registers registers;
} cpu_context;

#define CPU_FLAG_Z ((ctx & (1 << 7)) >> 7

void cpu_init();

#endif
