ENTRY(_start)                /* 定義入口點符號 */

SECTIONS
{
    /* 核心載入到物理位址 1MB 處 */
    . = 0x100000;

    /* 代碼段 */
    .text : ALIGN(4096) {
        *(.text)
        *(.text.*)          /* 包含編譯器生成的其他代碼區塊 */
    }

    /* 只讀數據段 */
    .rodata : ALIGN(4096) {
        *(.rodata)
        *(.rodata.*)
    }

    /* 數據段 */
    .data : ALIGN(4096) {
        *(.data)
        *(.data.*)
    }

    /* BSS 段（未初始化數據） */
    .bss : ALIGN(4096) {
        *(.COMMON)
        *(.bss)
        *(.bss.*)
    }

    /* 定義一個符號標記核心結束位置，方便之後做內存管理 */
    _kernel_end = .;
}