[bits 16]
switch_to_pm:
    cli                     ; 1. 關閉中斷
    lgdt [gdt_descriptor]   ; 2. 載入 GDT 指標

    mov eax, cr0            ; 3. 設定 CR0 暫存器的第 0 位元為 1
    or eax, 0x1
    mov cr0, eax

    jmp CODE_SEG:init_pm    ; 4. 長跳轉刷新流水線，正式進入 32-bit

[bits 32]
init_pm:
    mov ax, DATA_SEG        ; 5. 更新所有段暫存器
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000        ; 6. 重新設定堆疊
    mov esp, ebp

    ; 開始搬運：從 0x10000 搬到 0x100000
    mov esi, 0x10000          ; Source (64KB)
    mov edi, 0x100000         ; Destination (1MB)
    
    ; 計算要搬多少個 double words (4 bytes)
    ; 假設讀取了 KERNEL_SECTORS 個磁區
    mov eax, KERNEL_SECTORS
    imul eax, 512             ; 轉為 bytes
    shr eax, 2                ; 除以 4，得到 dword 數量
    mov ecx, eax
    
    cld
    rep movsd                 ; 暴力搬運    

    jmp 0x100000           ; 7. 跳轉至 32 位元主程式