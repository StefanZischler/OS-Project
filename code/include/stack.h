#ifndef STACK_H
#define STACK_H

#include <define.h>

void stack_push8(u8 value);
void stack_push16(u16 value);

u8 stack_pop8();
u16 stack_pop16();

#endif
