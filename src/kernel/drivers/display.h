#ifndef DISPLAY_H
#define DISPLAY_H
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f

#include "types.h"

// functions prototype
void kprint(char* message);
void clear_screen();
int handle_scrolling(int cursor_offset); // 新增：處理捲動
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_screen_offset(int col, int row);
void kprintf(char* format, ...);
void perform_backspace();
void set_backspace_min();
void draw_time_at_corner(u32 seconds);
#endif