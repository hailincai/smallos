#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

void init_keyboard();
void init_keyboard();

extern volatile int command_ready;
extern char key_buffer[256];
#endif