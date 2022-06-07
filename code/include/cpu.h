#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <instruction_set.h>

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
  instruction_set *current_instruction;
} cpu_context;

bool cpu_flag_Z();
bool cpu_flag_N();
bool cpu_flag_H();
bool cpu_flag_C();

void cpu_init();

#endif
