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

void cpu_set_flags(bool z, bool n, bool h, bool c) {
  /*if(z) {
    //set flag to true
    ctx.registers.f = ctx.registers.f | (1 << 7);
  } else {
    //set flag to false
    ctx.registers.f = ctx.registers.f & ~(1 << 7);
  }
  if(n) {
    ctx.registers.f = ctx.registers.f | (1 << 6);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 6);
  }
  if(h) {
    ctx.registers.f = ctx.registers.f | (1 << 5);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 5);
  }
  if(c) {
    ctx.registers.f = ctx.registers.f | (1 << 4);
  } else {
    ctx.registers.f = ctx.registers.f & ~(1 << 4);
  }*/
  cpu_set_flag(z, 7);
  cpu_set_flag(n, 6);
  cpu_set_flag(h, 5);
  cpu_set_flag(c, 4);
}

void cpu_set_flag(bool n, int offset) {
  if(n) {
    //set flag to true
    ctx.registers.f = ctx.registers.f | (1 << offset);
  } else {
    //set flag to false
    ctx.registers.f = ctx.registers.f & ~(1 << offset);
  }
}
