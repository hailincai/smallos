[bits 32]
; this is a external C method
[extern isr_handler]
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 32 ;timer interrupt
ISR_NOERRCODE 33 ;keyboard interrupt

isr_common_stub:
    pusha           ; save environment

    push esp   ; push registers_t 的指針
    call isr_handler
    add esp, 4

    popa            ; restore environment
    
    add esp, 8 ;remove int_no and err_code

    ; iret re-enable CPU level int processing
    iret    