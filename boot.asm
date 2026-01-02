[org 0x7c00]

    mov [BOOT_DRIVE], dl ; BIOS 啟動後會將磁碟代號存入 dl，先存起來備用

    ; 1. 設定堆疊 (Stack)
    mov bp, 0x8000       ; 避開 0x7c00 區域，找一塊安全的空間
    mov sp, bp

    ; 2. 準備讀取磁碟
    mov bx, 0x9000       ; 目標記憶體位址：es:bx = 0x0000:0x9000
    mov dh, 1            ; 設定讀取 1 個磁區
    mov dl, [BOOT_DRIVE] ; 使用剛才存起來的磁碟編號
    call disk_load

    ; 3. 驗證：印出 0x9000 處的資料
    mov si, 0x9000
    call print_string

    jmp $

; 引入模組
%include "print_string.asm"

; --- 磁碟讀取函式 ---
disk_load:
    push dx              ; 備份 dx (包含 dh=讀取數量)
    mov ah, 0x02         ; BIOS 讀取磁碟功能編號
    mov al, dh           ; 要讀取的磁區數量
    mov ch, 0x00         ; 磁柱編號 (0)
    mov dh, 0x00         ; 磁頭編號 (0)
    mov cl, 0x02         ; 從第 2 個磁區開始讀
    
    int 0x13             ; 呼叫 BIOS 中斷
    jc disk_error        ; 若 Carry Flag 為 1，跳轉錯誤處理

    pop dx
    cmp dh, al           ; 檢查實際讀取的數量 (al) 是否等於預期 (dh)
    jne disk_error
    ret

disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    jmp $

; 資料區
BOOT_DRIVE: db 0
DISK_ERR_MSG: db 'Disk read error!', 0

; --- 第 1 磁區結束標誌 ---
times 510-($-$$) db 0
dw 0xaa55

; --- 第 2 磁區內容 (位址將位於 0x9000) ---
test_data:
    db 'Data from Sector 2!', 0
times 512-($-test_data) db 0           ; 填充第 2 磁區，確保檔案大小正確