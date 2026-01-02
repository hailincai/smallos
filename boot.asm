[org 0x7c00]
    mov bp, 0x8000
    mov sp, bp
    call switch_to_pm      ; 一去不回頭的跳躍
    jmp $

%include "gdt.asm"
%include "switch_to_pm.asm"

[bits 32]
BEGIN_PM:
    mov edx, 0xb8000       ; VGA 顯示記憶體位址
    mov al, 'P'            ; 印出 'P' 代表 Protected Mode
    mov ah, 0x04           ; 黑底紅字屬性
    mov [edx], ax          ; 寫入顯示器
    jmp $

times 510-($-$$) db 0
dw 0xaa55