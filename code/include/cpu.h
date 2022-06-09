#ifndef CPU_H
#define CPU_H

#include <define.h>
#include <instruction_set.h>

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

  //current fetch data for instruction
  instruction_set *current_instruction;
  u8 current_opcode;
  u16 fetched_data;
  u16 memory_destination;
  bool destination_is_in_memory;

  //CPU information
  bool halted;
  bool stepping;

  //interrupt master flag
  bool interrupt_master_enabled_flag;  

  //interrupt enable register (IE)
  u8 ie_register;

  
  //interrupts
  bool ime;
  u8 interrupt_flag;
} cpu_context;

bool cpu_flag_Z();
bool cpu_flag_N();
bool cpu_flag_H();
bool cpu_flag_C();

void cpu_set_flags(bool z, bool n, bool h, bool c);
void cpu_set_flag(bool n, int offset);

void cpu_init();
bool cpu_step();


u8 cpu_get_ie_register();
void cpu_set_ie_register(u8 value);

typedef void (*INS_TYPE)(cpu_context *);

INS_TYPE instruction_get_type(instruction_type type);

void fetch_instruction();
void fetch_instruction_data();

u16 instruction_read_register(register_type register_type);


#endif
