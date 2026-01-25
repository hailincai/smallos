/* user/user.ld */
OUTPUT_FORMAT("binary")    /* 直接輸出純二進制格式，方便內核讀取 */
ENTRY(_start)

SECTIONS
{
    . = 0x400000;          /* 必須與內核映射的 USER_CODE_VADDR 一致 */

    .text : { *(.text) }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}