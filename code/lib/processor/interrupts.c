#include <stdio.h>

#include <interrupts.h>
#include <cpu.h>
#include <stack.h>

void interrupt_handle(cpu_context *ctx, interrupt_type type, u16 address);

//requests an interrupt by setting the corresponding interrupt flag
void cpu_request_interrupt(cpu_context *ctx, interrupt_type type) {
  //printf("Interrupt request %d\n", type);
  cpu_get_context()->interrupt_flag = cpu_get_context()->interrupt_flag | type;
}

//handles requested interrupts if they are enabled
void cpu_handle_interrupts(cpu_context *ctx) {
  //if at least one interrupt is pending, the cpu wakes up
  //even if IME is not set
  if(ctx->interrupt_flag) {
    ctx->halted = false;
  }

  //if IME is false, no interrupts may be handled
  
  if(ctx->interrupt_flag & INTERRUPT_VBLANK && ctx->ie_register & INTERRUPT_VBLANK) {
    interrupt_handle(ctx, INTERRUPT_VBLANK, 0x40);
    return;
  }
  if(ctx->interrupt_flag & INTERRUPT_LCD_STAT && ctx->ie_register & INTERRUPT_LCD_STAT) {
    interrupt_handle(ctx, INTERRUPT_LCD_STAT, 0x48);
    return;
  }
  if(ctx->interrupt_flag & INTERRUPT_TIMER && ctx->ie_register & INTERRUPT_TIMER) {
    interrupt_handle(ctx, INTERRUPT_TIMER, 0x50);
    return;
  }
  if(ctx->interrupt_flag & INTERRUPT_SERIAL && ctx->ie_register & INTERRUPT_SERIAL) {
    interrupt_handle(ctx, INTERRUPT_SERIAL, 0x58);
    return;
  }
  if(ctx->interrupt_flag & INTERRUPT_JOYPAD && ctx->ie_register & INTERRUPT_JOYPAD) {
    interrupt_handle(ctx, INTERRUPT_JOYPAD, 0x60);
    return;
  }

}

//helper method
void interrupt_handle(cpu_context *ctx, interrupt_type type, u16 address) {
  printf("handle interrupt %d\n", type);
  //reset IME and interrupt flag for interrupt type
  ctx->interrupt_master_enabled_flag = false;
  ctx->interrupt_flag &= ~INTERRUPT_VBLANK;
  
  ctx->halted = false;
  
  //set the program counter to address of the handler
  stack_push16(ctx->registers.pc);
  ctx->registers.pc = address;
}
