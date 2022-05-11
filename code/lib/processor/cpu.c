#include <cpu.h>

cpu_context ctx = {0};

void cpu_init() {
  ctx.registers.pc = 0x100;
}
