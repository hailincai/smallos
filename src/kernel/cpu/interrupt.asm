[bits 32]
; this is a external C method
[extern irq_handler]
; 時鐘中斷
global irq0 ; 時鐘中斷
irq0:
    pusha           ; save environment
    push dword 0    ; tell irq_handler, this is hardware int 0
    call irq_handler
    add esp, 4      ; remove the pushed flag
    popa            ; restore environment
    ; iret re-enable CPU level int processing
    iret; 鍵盤中斷

global irq1 ; this is the hardware interrupt 1 (keyboard)
irq1:
    pusha           ; save environment
    push dword 1    ; tell irq_handler, this is hardware int 1
    call irq_handler
    add esp, 4      ; remove the pushed flag
    popa            ; restore environment
    ; iret re-enable CPU level int processing
    iret