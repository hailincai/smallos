#include "keyboard.h"
#include "io.h"
#include "display.h"
#include "isr.h"

// variables only visible in keyboard.c
static int shift_pressed = 0;
static int capslock_pressed = 0;
char key_buffer[256];
static int buffer_idx = 0;

// global variables exposed by keyboard.c
volatile int command_ready = 0;


// 美規標準鍵盤映射表
// 索引即為 Scan Code
const char ascii_nomod[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// Shift 映射表
const char ascii_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

void keyboard_callback(u32 interrupt_no);

void init_keyboard() 
{
    // 讀取並丟棄所有當前鍵盤緩衝區的數據
    // this is for reboot
    while (port_byte_in(0x64) & 0x01) {
        port_byte_in(0x60);
    }  
    command_ready = 0;
    key_buffer[0] = '\0';      
    register_interrupt_handler(0x01, keyboard_callback);
}

void keyboard_callback(u32) {
    // 讀取掃描碼
    u8 scancode = port_byte_in(0x60);

    if (scancode == 0x3A)
    {
        capslock_pressed = capslock_pressed ^ 1;
        return;
    }

    // 檢查是否為 Shift 鍵的掃描碼
    // 左 Shift: 0x2A, 右 Shift: 0x36
    if (scancode == 0x2A || scancode == 0x36) 
    {
        shift_pressed = 1;
        return;
    }

    // 放開 Shift (掃描碼 = 按下碼 + 0x80)
    // 左放開: 0xAA, 右放開: 0xB6
    if (scancode == 0xAA || scancode == 0xB6) 
    {
        shift_pressed = 0;
        return;
    }    

    // 如果掃描碼最高位是 1 (>= 0x80)，代表按鍵放開 (Key Released)
    if (scancode & 0x80) 
    {
        // 目前暫不處理放開事件
        return;
    }

    if (scancode == 0x0E) 
    { // Backspace
        if (buffer_idx > 0)
        {
            buffer_idx --;
            perform_backspace();
        }
    } else if (scancode == 0x1C) 
    { // Enter
        // a command is ready
        key_buffer[buffer_idx] = '\0';
        command_ready = 1;
        buffer_idx = 0;
        kprint("\n");
        set_backspace_min();
    } else if (scancode <= 0x39) 
    {
        char letter = shift_pressed ? ascii_shift[(int)scancode] : ascii_nomod[(int)scancode];

        // for letter, we need to combin with capslock_pressed
        if (('a' <= letter && letter <= 'z') || ('A' <= letter && letter <= 'Z'))
        {
            if (capslock_pressed == 1)
            {
                letter = (shift_pressed) ? ascii_nomod[(int)scancode] : ascii_shift[(int)scancode];
            }else
            {
                letter = (shift_pressed) ? ascii_shift[(int)scancode] : ascii_nomod[(int)scancode];
            }
        }
        
        if (letter != 0 && buffer_idx < 255 && command_ready == 0) {
            key_buffer[buffer_idx++] = letter;
            char str[2] = {letter, '\0'};
            kprint(str);
        }
    }
}