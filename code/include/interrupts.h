#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <cpu.h>

typedef enum {
  INTERRUPT_VBLANK = 1,
  INTERRUPT_LCD_STAT = 2,
  INTERRUPT_TIMER = 4,
  INTERRUPT_SERIAL = 8,
  INTERRUPT_JOYPAD = 16
} interrupt_type;

void cpu_request_interrupt(interrupt_type type);

void cpu_handle_interrupts(cpu_context *ctx);

#endif
