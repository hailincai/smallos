SECTIONS
{
    . = 0x1000;

    .text : { *(.text) }
    .rodata : { *(.rodata) }

    /* 強制數據段從下一個 4KB 邊界開始 */
    .data : ALIGN(4096) {
        *(.data)
    }

    .bss : ALIGN(4096) {
        *(.COMMON)
        *(.bss)
    }
}