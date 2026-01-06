// src/kernel/io.c

/**
 * 從指定的 I/O 埠讀取一個位元組
 */
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    // __asm__ ("指令" : "=a" (輸出) : "d" (輸入))
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

/**
 * 向指定的 I/O 埠寫入一個位元組
 */
void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

/**
 * Write a word to register
 */
void port_word_out(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}