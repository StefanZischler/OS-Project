#include <timer.h>

static timer_context ctx = {0};

void timer_init() {
  
}

void timer_tick() {
  //16384 Hz
  u16 prev_div = ctx.div;
  ctx.div++;
  
  bool update_timer = false;
  
  //check for timer increase
  //TAC determines which frequency is used
  //for more information check gbdev.io/pandocs/Timer_Obscure_Behaviour.html
  switch(ctx.tac & 0x03) {
    case 0x00:  //00 -> 4096 Hz
      update_timer = (!(ctx.div & (1 << 9))) && (prev_div & (1 << 9));
      break;
    case 0x01:  //01 -> 262144 Hz
      update_timer = (!(ctx.div & (1 << 3))) && (prev_div & (1 << 3));
      break;
    case 0x02:  //10 -> 65536 Hz
      update_timer = (!(ctx.div & (1 << 5))) && (prev_div & (1 << 5));
      break;
    case 0x03:  //11 -> 16384 Hz
      update_timer = (!(ctx.div & (1 << 7))) && (prev_div & (1 << 7));
      break;
  }
  
  //check if timer should increase with current frequency
  //and if the timer is currently enabled
  if(update_timer && (ctx.tac & (1 << 2))) {
    ctx.tima++;
    
    //check if TIMA overflowed
    if(ctx.tima == 0) {
      //reset TIMA
      ctx.tima = ctx.tma;
      
      //TODO: interrupt request
    }
  }
}

u8 timer_read(u16 address) {
  switch(address) {
    case 0xFF04:  //DIV (divider register)
      
    case 0xFF05:  //TIMA (timer counter)
      return ctx.tima;
    case 0xFF06:  //TMA (timer modulo)
      return ctx.tma;
    case 0xFF07:  //TAC (timer control)
      return ctx.tac;
  }
  return 0;
}

void timer_write(u16 address, u8 value) {
  switch(address) {
    case 0xFF04:  //DIV (divider register)
      ctx.div = 0;  //reset DIV
      break;
    case 0xFF05:  //TIMA (timer counter)
      ctx.tima = value;
      break;
    case 0xFF06:  //TMA (timer modulo)
      ctx.tima = value;
      break;
    case 0xFF07:  //TAC (timer control)
      ctx.tima = value;
      break;
  }
}
