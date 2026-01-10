[org 0x7c00]
    KERNEL_OFFSET equ 0x1000    ; 核心載入的目標位址
    MEM_MAP_ADDR equ 0x8000     ; BIOS記錄的內存mapping
    
    mov [BOOT_DRIVE], dl        ; BIOS 把啟動磁碟號存放在 DL
    mov bp, 0x9000              ; 設定棧
    mov sp, bp

    ; 1. 印出啟動訊息
    mov si, MSG_REAL_MODE
    call print_string

    ; 2. open a20
    in al, 0x92
    or al, 2
    out 0x92, al  ; 這是 Fast A20，通常 QEMU 必中    

    ; 3. 從bios導入內存映射
    call load_mem_map

    ; 4. 載入核心 (使用 LBA 模式)
    call load_kernel

    ; 5. 切換到保護模式
    call switch_to_pm

    jmp $

%include "gdt.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"

[bits 16]
load_mem_map:
mov di, MEM_MAP_ADDR + 4    ; 留出前 4 字節存入 entry 數量
    xor ebx, ebx                ; ebx 必須初始化為 0
    xor bp, bp                  ; bp 用來計數 entry 數量
    mov edx, 0x534D4150         ; 'SMAP' 簽名
.loop:
    mov eax, 0xE820
    mov ecx, 24                 ; 請求 24 字節 (ACPI 3.0)
    int 0x15
    jc .done                    ; Carry flag 代表結束
    cmp eax, 0x534D4150         ; 檢查簽名
    jne .error
    
    add di, 24                  ; 移動指針到下一個結構
    inc bp                      ; 數量 + 1
    test ebx, ebx               ; 如果 ebx 為 0，代表結束
    jnz .loop
.done:
    mov [MEM_MAP_ADDR], bp      ; 將總數量存在開頭
    ret
.error:
    ; 處理錯誤
    mov si, MEM_MAP_LOAD_ERR_MSG
    call print_string
    jmp $

[bits 16]
load_kernel:
    mov si, MSG_LOAD_KERNEL
    call print_string

    ; 設定 DAP 結構內容
    mov word [dap.count], KERNEL_SECTORS   ; Makefile 傳入的磁區數量
    mov word [dap.offset], 0x0000    ; 0x1000
    mov word [dap.segment], 0x1000          ; 段位址
    mov dword [dap.lba_low], 1              ; 從 LBA 1 開始 (LBA 0 是引導磁區)

    ; 呼叫 LBA 讀取
    mov dl, [BOOT_DRIVE]
    mov si, dap                             ; DS:SI 指向 DAP
    mov ah, 0x42                            ; 擴展讀取功能
    int 0x13
    
    jc disk_error                           ; 如果 Carry Flag 被置位，跳轉錯誤
    mov si, DISK_SUCC_MSG
    call print_string
    ret

[bits 16]
disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    ; 如果 AH 有值，通常是錯誤碼，這裡暫時直接停機
    jmp $

; ---------------------------------------------------------
; Disk Address Packet (DAP) 結構
; ---------------------------------------------------------
align 4
dap:
    db 0x10                ; DAP 大小 (16 bytes)
    db 0x00                ; 保留 (始終為 0)
    .count   dw 0          ; 要讀取的磁區數量
    .offset  dw 0          ; 目標地址偏移 (Offset)
    .segment dw 0          ; 目標地址段 (Segment)
    .lba_low dd 0          ; 起始 LBA 低 32 位元
    .lba_high dd 0         ; 起始 LBA 高 32 位元 (通常為 0)

; ---------------------------------------------------------
; 數據區
; ---------------------------------------------------------
BOOT_DRIVE      db 0
MSG_REAL_MODE   db "Started in 16-bit Real Mode", 0x0d, 0x0a, 0
MSG_LOAD_KERNEL db "Loading kernel via LBA...", 0x0d, 0x0a, 0
DISK_ERR_MSG    db "LBA Disk read error!", 0
DISK_SUCC_MSG   db "LBA disk read succ!", 0
MEM_MAP_LOAD_ERR_MSG db "Loading memory map fail!", 0

times 510-($-$$) db 0
dw 0xaa55