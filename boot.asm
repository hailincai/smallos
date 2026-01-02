[org 0x7c00]

; --- 主程式區段 ---
mov si, HELLO_MSG    ; 將字串的首位址交給 SI
call print_string    ; 呼叫函式

mov si, BYE_MSG      ; 切換到另一個字串
call print_string

jmp $                ; 無限循環，卡住 CPU

; --- 函式：print_string ---
; 目的：印出以 0 結尾的字串
print_string:
    pusha            ; 保護暫存器現場
.loop:
    lodsb            ; 從 SI 讀取到 AL，SI 自動遞增
    cmp al, 0        ; 判斷是否為字串結尾 (Null Terminator)
    je .done         ; 如果是 0，跳轉到結束標籤
    
    mov ah, 0x0e     ; BIOS 電傳打字模式
    int 0x10         ; 呼叫中斷顯示字元
    jmp .loop        ; 繼續循環處理下一個字元
.done:
    popa             ; 還原暫存器現場
    ret              ; 返回主程式

; --- 資料區段 ---
HELLO_MSG:
    db 'Hello, OS World!', 0x0d, 0x0a, 0  ; 0x0d, 0x0a 是換行與回車
BYE_MSG:
    db 'Successfully printed string!', 0x0d, 0x0a, 0

; --- 引導扇區填充分區 ---
times 510-($-$$) db 0
dw 0xaa55