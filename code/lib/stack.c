#include <stack.h>
#include <cpu.h>
#include <bus.h>

extern cpu_context ctx;

//push operations move the stack pointer BEFORE writing the value
void stack_push8(u8 value) {
  ctx.registers.sp--;
  bus_write(ctx.registers.sp, value);
}

void stack_push16(u16 value) {
  stack_push8((value >> 8));
  stack_push8(value);
}

//pop operations move the stack pointer AFTER retrieving the current value
u8 stack_pop8() {
  return bus_read(ctx.registers.sp++);
}

u16 stack_pop16() {
  u8 lo = stack_pop8();
  u8 hi = stack_pop8();
  return (hi << 8) | lo;
}
