[org 0x7c00]
    KERNEL_OFFSET equ 0x1000    ; 核心載入的目標位址
    mov [BOOT_DRIVE], dl    ; BIOS 會把啟動磁碟號碼存在 DL，先存起來備用

    mov bp, 0x9000 ;stack for real mode
    mov sp, bp

    ; 1. 印出啟動訊息 (16-bit)
    mov si, MSG_REAL_MODE
    call print_string

    ; 2. 載入核心 (從磁碟讀取)
    call load_kernel

    ; 3. 切換到保護模式 (一去不回頭)
    call switch_to_pm      ; 這裡一去不回頭

    jmp $
%include "gdt.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"

[bits 16]
load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET   ; 設定讀取目標位址：ES:BX = 0x0000:0x1000
    mov dh, KERNEL_SECTORS  ; 預計讀取 15 個磁區 (目前的 kernel.c 很小，15 綽綽有餘)
    mov dl, [BOOT_DRIVE]    ; 使用剛才存好的磁碟號
    call disk_load          ; 調用你之前的磁碟讀取函式
    ret

[bits 16]
; --- 磁碟讀取函式 (如果沒拆分出去，就放在這) ---
disk_load:
    pusha            ; 備份所有暫存器 (重要！)
    push dx          ; 備份傳入的 dx (包含 dh 讀取數量)

    mov ah, 0x02     ; BIOS 讀取磁區功能
    mov al, dh       ; 讀取 dh 個磁區
    mov ch, 0x00     ; 磁柱 0
    mov dh, 0x00     ; 磁頭 0
    mov cl, 0x02     ; 從第 2 磁區開始 (第 1 磁區是 Bootloader)

    int 0x13         ; 呼叫 BIOS 中斷
    jc disk_error    ; 如果進位標誌 (Carry Flag) 為 1，代表出錯

    pop dx           ; 還原當初要求的 dx
    cmp dh, al       ; 檢查實際讀取的數量 (al) 是否等於要求的數量 (dh)
    jne disk_error
    
    popa
    ret

disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    jmp $

[bits 32]
BEGIN_PM:
    ; 到這裡時，GDT 已經設定好，段暫存器也已經更新
    ; 我們直接跳轉到 C 核心被載入的位置執行
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE      db 0
MSG_REAL_MODE   db "Started in 16-bit Real Mode", 0x0d, 0x0a, 0
MSG_LOAD_KERNEL db "Loading kernel into memory...", 0x0d, 0x0a, 0
DISK_ERR_MSG db "Disk read error!", 0

times 510-($-$$) db 0
dw 0xaa55