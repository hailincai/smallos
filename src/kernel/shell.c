#include "string.h"
#include "display.h"
#include "types.h"
#include "shell.h"
#include "io.h"
#include "keyboard.h"
#include "cmos.h"

void shell_print_prompt() 
{
    kprint("\nSmallOS > ");
}

/**
 * 曾對QEMU的shutdown指令，不一定適合真實機器
 */
void shutdown() {
    kprint("Shutting down Small OS...\n");
    
    // 方法 A: QEMU 專用的 ACPI 關機 (適用於多數 QEMU 預設配置)
    // 寫入 0x2000 到 0x604 連接埠
    port_word_out(0x604, 0x2000);

    // 方法 B: 較舊版本或特定 BIOS 的關機
    port_word_out(0xB004, 0x2000);

    // 方法 C: 如果是在真實機器上，這通常會觸發「三倍故障 (Triple Fault)」
    // 雖然不是優雅的關機，但在沒實作 ACPI 的情況下，這是讓機器停止運行的最後手段
    kprint("Shutdown failed. Halting CPU.\n");
    __asm__ __volatile__("cli");
    while(1) { __asm__ __volatile__("hlt"); }
}

/**
 * 這個是熱啟動，只有CPU被復位
 */
void reboot() 
{
    kprint("Rebooting system...\n");

    // 1. 關閉所有中斷，防止在重啟過程中被干擾
    __asm__ __volatile__("cli");

    // 2. 向鍵盤控制器發送重啟命令
    u8 temp;
    // 等待鍵盤控制器緩衝區清空
    do {
        temp = port_byte_in(KEYBOARD_STATUS_REG);
    } while (temp & 0x02);
    
    port_byte_out(KEYBOARD_COMMAND_REG, 0xFE);

    // 3. 關鍵：如果硬體重啟慢了，我們絕對不能讓 CPU 執行後面的代碼
    // 進入一個死循環，原地等待硬體重置
    while(1) {
        __asm__ __volatile__("hlt");
    }    
}

/**
 * 執行shell指令
 */
void shell_execute(char *input)
{
    if (k_str_len(input) == 0) return;

    char *args = k_str_split_first(input, ' ');

    input = k_to_lowercase(input);

    if (k_str_cmp(input, "help") == 0)
    {
        kprint("SmallOS Shell. Available commands: help, echo, clear, reboot, shutdown");
    }else if (k_str_cmp(input, "echo") == 0)
    {
        if (args != NULL)
        {
            kprint(args);
        }
    }else if (k_str_cmp(input, "clear") == 0)
    {
        clear_screen();
        set_cursor_offset(get_screen_offset(0, 0));
    }else if (k_str_cmp(input, "reboot") == 0) 
    {
        reboot();
    }else if (k_str_cmp(input, "shutdown") == 0)
    {
        shutdown();
    }else if (k_str_cmp(input, "time") == 0)
    {
        char timestr[20];
        get_rtc_time_string(timestr);
        kprintf("Current time is %s in UTC", timestr);
    }
    else
    {
        kprintf("Unknow command: %s\n", input);        
    }
}