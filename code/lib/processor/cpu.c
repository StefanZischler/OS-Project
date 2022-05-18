#include <stdio.h>
#include <stdbool.h>

#include <cpu.h>

cpu_context ctx = {0};

void cpu_init() {
  //set program counter to initial value
  ctx.registers.pc = 0x100;
  
  //set stack pointer to initial value
  ctx.registers.sp = 0xFFFE;
  
  //print values for debugging
  printf("pc: %d\n", ctx.registers.pc);
  printf("sp: %d\n", ctx.registers.sp);
  printf("Z-flag: %d\n", cpu_flag_Z());
  printf("N-flag: %d\n", cpu_flag_N());
  printf("H-flag: %d\n", cpu_flag_H());
  printf("C-flag: %d\n", cpu_flag_C());
  
}

bool cpu_flag_Z() {
  return (ctx.registers.f & (1 << 7)) >> 7;
}

bool cpu_flag_N() {
  return (ctx.registers.f & (1 << 6)) >> 6;
}

bool cpu_flag_H() {
  return (ctx.registers.f & (1 << 5)) >> 5;
}

bool cpu_flag_C() {
  return (ctx.registers.f & (1 << 4)) >> 4;
}
