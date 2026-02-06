#ifndef TIMER_H
#define TIMER_H
#include "types.h"

#define TIMER_CONTROL_REG 0x43
#define COUNTR_0_REG 0x40

void init_timer(u32 freq);
u32 get_ticker();
void sleep(u32 ticksToWait);
#endif