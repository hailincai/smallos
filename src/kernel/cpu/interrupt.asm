[bits 32]
; this is a external C method
[extern isr_handler]
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push 0
    push %1
    jmp isr_common_stub
%endmacro

; 對於某些cpu中斷，cpu在調用isr 程序之前，會自動壓入一個error code
%macro ISR_WITHERRCODE 1
global isr%1
isr%1:
    cli
    push %1
    jmp isr_common_stub
%endmacro

ISR_WITHERRCODE 14 ;PAGE FAULT EXCEPTION
ISR_NOERRCODE 32 ;timer interrupt
ISR_NOERRCODE 33 ;keyboard interrupt
ISR_NOERRCODE 128 ; int 0x80 interrupt, user mode to execute kernel mode functions

isr_common_stub:
    pusha           ; save environment

mov ax, ds               ; 獲取當前數據段
    push eax                 ; 壓入 ds (現在所有中斷都會壓入這個了！)

    mov ax, 0x10             ; 切換到內核數據段
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax    

    push esp   ; push registers_t 的指針
    call isr_handler
    add esp, 4

    pop eax                  ; 彈出 ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax    

    popa            ; restore environment
    
    add esp, 8 ;remove int_no and err_code

    ; iret re-enable CPU level int processing
    iret    


