[org 0x7c00]          ; 告訴編譯器，這段代碼預計會被載入到記憶體地址 0x7c00

mov ah, 0x0e          ; BIOS 中斷服務 10h：顯示字元 (Teletype mode)
mov al, 'H'           ; 將字元 'H' 存入 al 寄存器
int 0x10              ; 呼叫中斷，由 BIOS 負責把字元印在螢幕上
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10

jmp $                 ; $ 表示當前地址，這行會讓 CPU 在這裡無限循環，防止跑飛

; 填充與簽名
times 510-($-$$) db 0 ; 用 0 填充剩餘空間，確保檔案總大小剛好為 510 字節
dw 0xaa55             ; 最後 2 字節：寫入啟動標誌 (0x55, 0xAA)