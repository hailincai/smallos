#include "keyboard.h"
#include "io.h"
#include "display.h"
#include "isr.h"
#include "keyboard.h"

// 美規標準鍵盤映射表
// 索引即為 Scan Code
const char ascii_nomod[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void keyboard_callback(u32) {
    // 讀取掃描碼
    u8 scancode = port_byte_in(0x60);

    // 如果掃描碼最高位是 1 (>= 0x80)，代表按鍵放開 (Key Released)
    if (scancode & 0x80) 
    {
        // 目前暫不處理放開事件
        return;
    }

    if (scancode <= 0x39) 
    {
        char letter = ascii_nomod[(int)scancode];
        char str[2];
        str[0] = letter;
        str[1] = '\0';
        kprint(str);
    }
}

void init_keyboard() 
{
    // 讀取並丟棄所有當前鍵盤緩衝區的數據
    // this is for reboot
    while (port_byte_in(0x64) & 0x01) {
        port_byte_in(0x60);
    }  
    register_interrupt_handler(0x01, keyboard_callback);
}