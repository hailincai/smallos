ENTRY(_start)

SECTIONS
{
    /* VMA (虛擬地址) 從 3GB + 1MB 開始 */
    . = 0xC0100000;

    /* --- 代碼段 --- */
    /* LMA (物理地址) 強制從 0x100000 開始 */
    .text : AT(0x100000) ALIGN(4096)
    {
        *(.text)
    }

    /* --- 只讀數據段 --- */
    /* 這裡的 AT 計算會自動對應到物理地址 1MB + .text 的大小 */
    .rodata : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4096)
    {
        *(.rodata)
    }

    /* --- 數據段 --- */
    .data : AT(ADDR(.data) - 0xC0000000) ALIGN(4096)
    {
        *(.data)
    }

    /* --- BSS 段 --- */
    .bss : AT(ADDR(.bss) - 0xC0000000) ALIGN(4096)
    {
        *(.COMMON)
        *(.bss)
    }

    _kernel_end = .;
}