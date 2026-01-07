#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEYBOARD_STATUS_REG 0x64
#define KEYBOARD_COMMAND_REG 0x64
#define KEYBOARD_DATA_REG 0x60

void init_keyboard();
void init_keyboard();

extern volatile int command_ready;
extern char key_buffer[256];
#endif