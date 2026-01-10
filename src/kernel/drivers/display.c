#include "io.h" 
#include "util.h"
#include <stdarg.h>
#include "display.h"
#include "types.h"
#include "mem.h"

// VGA 控制埠
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

static int backspace_min_offset = 0; // 靜態變數，記錄最小可退格位置

// 當 kernel 輸出完 "> " 後，呼叫這個函式來鎖定位置
void set_backspace_min() {
    backspace_min_offset = get_cursor_offset();
}

// 輔助函式：計算螢幕偏移量
int get_screen_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

// 簡易記憶體拷貝
void memory_copy(char* source, char* dest, int no_bytes) {
    for (int i = 0; i < no_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

int handle_scrolling(int cursor_offset) {
    // 檢查是否超出螢幕 (25 * 80 * 2 = 4000 bytes)
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }

    // 搬移記憶體
    for (int i = 1; i < MAX_ROWS; i++) {
        memory_copy(
            (char*)(VIDEO_VIRT_ADDR + get_screen_offset(0, i)),
            (char*)(VIDEO_VIRT_ADDR + get_screen_offset(0, i - 1)),
            MAX_COLS * 2
        );
    }

    // 清除最後一行
    char* last_line = (char*)(VIDEO_VIRT_ADDR + get_screen_offset(0, MAX_ROWS - 1));
    for (int i = 0; i < MAX_COLS * 2; i += 2) {
        last_line[i] = ' ';
        last_line[i+1] = WHITE_ON_BLACK;
    }

    // 重要：回傳最後一行的起始位置
    return get_screen_offset(0, MAX_ROWS - 1);
}

void clear_screen() {
    char* video_memory = (char*) VIDEO_VIRT_ADDR;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = WHITE_ON_BLACK;
    }
}

/**
 * 從硬體獲取目前的游標偏移量 (以 byte 為單位)
 */
int get_cursor_offset() {
    // 告訴 VGA 我們要讀取索引 14 (高位位元組)
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    // 告訴 VGA 我們要讀取索引 15 (低位位元組)
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    // VGA 回傳的是「字元數」，乘以 2 轉為「位元組偏移量」
    return offset * 2;
}

/**
 * 將新的偏移量寫回硬體游標
 */
void set_cursor_offset(int offset) {
    offset /= 2; // 位元組轉回字元位置
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

// 修改後的 kprint，自動接續游標印字
void kprint(char* message) {
    int offset = get_cursor_offset();
    char* video_memory = (char*) VIDEO_VIRT_ADDR;

    for (int i = 0; message[i] != 0; i++) {
        // 處理手動換行符 '\n'
        if (message[i] == '\n') {
            // 移到下一行的開頭： (當前行數 + 1) * 每行寬度 * 2
            offset = get_screen_offset(0, (offset / (MAX_COLS * 2)) + 1);
        } else {
            video_memory[offset] = message[i];
            video_memory[offset + 1] = WHITE_ON_BLACK;
            offset += 2;
        }
        
        // 檢查是否需要捲動
        offset = handle_scrolling(offset);
    }
    set_cursor_offset(offset);
}

void kprintf(char* format, ...) {
    va_list args;
    va_start(args, format);

    char* ptr;
    char buffer[32];

    for (ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr != '%') {
            char temp[2] = {*ptr, '\0'};
            kprint(temp);
            continue;
        }

        ptr++; // 跳過 '%'
        switch (*ptr) {
            case 's': // 字串
                kprint(va_arg(args, char*));
                break;
            case 'd': // 十進位
                int_to_ascii(va_arg(args, int), buffer);
                kprint(buffer);
                break;
            case 'x': // 十六進位
                hex_to_ascii(va_arg(args, int), buffer);
                kprint(buffer);
                break;
            case '%':
                kprint("%");
                break;
        }
    }
    va_end(args);
}

void perform_backspace() 
{
    int offset = get_cursor_offset();
    // 如果當前位置已經等於或小於限制點，直接返回，不執行刪除
    if (offset <= backspace_min_offset) return;

    // last char is at the position of offset - 2
    offset -= 2;
    u8 *videoMemo = (u8*)0xb8000;
    videoMemo[offset] = ' '; //char
    videoMemo[offset + 1] = WHITE_ON_BLACK; //color
    set_cursor_offset(offset);
}

void draw_time_at_corner(u32 seconds)
{
    char time_str[16] = "Uptime: ";
    char num_str[8];

    int_to_ascii(seconds, num_str);

    // 取得顯存位址 (第 0 行，第 65 列開始)
    // VGA 模式每行 80 個字元，每個字元 2 bytes
    u8 *vidmem = (u8*)0xb8000;
    int offset = (0 * 80 + 65) * 2;    
    // 先印 "Uptime: "
    for (int k = 0; time_str[k] != '\0'; k++) {
        vidmem[offset++] = time_str[k];
        vidmem[offset++] = 0x0E; // 黃色字
    }
    // 再印數字
    for (int k = 0; num_str[k] != '\0'; k++) {
        vidmem[offset++] = num_str[k];
        vidmem[offset++] = 0x0E;
    }    
}