#include "io.h"
#include "display.h"
#include "types.h"
#include "keyboard.h"
#include "isr.h"

void (*interrupt_handlers[256])(u32);

void register_interrupt_handler(u8 n, void (*handler)(u32)) {
    interrupt_handlers[n] = handler;
}

void irq_handler(u32 interrupt_no) {
    // 呼叫已註冊的處理函式
    if (interrupt_handlers[interrupt_no] != 0) {
        interrupt_handlers[interrupt_no](interrupt_no);
    }
    
    // 重要：發送 EOI (End of Interrupt) 給 PIC，否則它不會再發送下一次中斷
    // enable PIC level interrupt for same level or low level
    port_byte_out(ISR_MASTER_CONTROL_REG, 0x20); // 向主 PIC 發送    
}

void init_pic() {
    // 初始化主從 PIC
    port_byte_out(ISR_MASTER_CONTROL_REG, 0x11);
    port_byte_out(ISR_SLAVE_CONTROL_REG, 0x11);
    // 重新映射偏移量, 因為默認的中斷號和CPU內部定義的中斷衝突
    port_byte_out(ISR_MASTER_MASK_REG, 0x20); // 主 PIC 映射到 32-39
    port_byte_out(ISR_SLAVE_MASK_REG, 0x28); // 從 PIC 映射到 40-47
    // 其他初始化指令...
    port_byte_out(ISR_MASTER_MASK_REG, 0x04);
    port_byte_out(ISR_SLAVE_MASK_REG, 0x02);
    port_byte_out(ISR_MASTER_MASK_REG, 0x01);
    port_byte_out(ISR_SLAVE_MASK_REG, 0x01);
    // 開啟鍵盤中斷 (遮罩)
    port_byte_out(ISR_MASTER_MASK_REG, 0xFC); // 11111100 (只留 IRQ 1, IRQ 0)
    port_byte_out(ISR_SLAVE_MASK_REG, 0xFF); // Disable all interrupts from secondary PIC
}