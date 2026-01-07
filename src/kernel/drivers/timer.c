#include "timer.h"
#include "io.h"
#include "isr.h"
#include "display.h"

static u32 tick = 0; // 記錄系統啟動以來跳動了多少次

u32 get_ticker()
{
    return tick;
}

void sleep(u32 ticksToWait)
{
    u32 endTick = tick + ticksToWait;
    while (tick < endTick)
    {
        //make cpu goes to hlt mode, and will come back when next interval, like timer comes
        __asm__ __volatile__("hlt");
    }
}

static void timer_callback(u32) {
    tick++;

    if (tick % 100 == 0)
    {
        // 每秒更新一次螢幕右上角
        u32 seconds = tick / 100;
        draw_time_at_corner(seconds);
    }
}

void init_timer(u32 freq) {
    tick = 0;
    
    // 註冊 IRQ 0 (中斷號 32)
    register_interrupt_handler(0, timer_callback);

    // 計算除數
    u32 divisor = 1193180 / freq;

    // 發送命令字 (0x36 代表：通道0、存取LOBYTE/HIBYTE、模式3平方波、二進制)
    port_byte_out(TIMER_CONTROL_REG, 0x36);

    // 分兩次發送除數的低 8 位元與高 8 位元
    port_byte_out(COUNTR_0_REG, (u8)(divisor & 0xFF));
    port_byte_out(COUNTR_0_REG, (u8)((divisor >> 8) & 0xFF));
}