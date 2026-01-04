gdt_start:
; the GDT table requires an empty GDT as first entry
gdt_null:        ; 1. 強制性的空描述符 (8 bytes)
    dd 0x0
    dd 0x0

gdt_code:        ; 2. 程式碼段描述符
    dw 0xffff    ; Limit (0-15 bits)
    dw 0x0       ; Base (0-15 bits)
    db 0x0       ; Base (16-23 bits)
    db 10011010b ; Access Byte (Present, Ring 0, Code, Exec/Read)
    db 11001111b ; Flags + Limit (4KB granularity, 32-bit)
    db 0x0       ; Base (24-31 bits)

gdt_data:        ; 3. 資料段描述符
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b ; Access Byte (Present, Ring 0, Data, Read/Write)
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; GDT 大小
    dd gdt_start               ; GDT 起始位址

; 常數定義，方便段暫存器載入
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start