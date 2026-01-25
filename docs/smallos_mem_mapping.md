虛擬地址起始,長度,用途,權限 (Flags),說明
0x00000000,4MB,NULL / Reserved,-,捕獲空指針異常
0x00400000,4KB,User Code,0x07 (U/R/P),映射第一個用戶測試函數
0x00800000,4KB,User Stack,0x07 (U/R/P),用戶態運行時棧空間
...,-,Available,-,用戶空間預留區域
0xC0000000,4MB,Kernel Image,0x03 (K/R/P),內核代碼段與數據段
0xD0000000,1MB,Kernel Heap,0x03 (K/R/P),內核動態內存管理範圍
0xE0000000,-,MMIO / Reserved,0x03 (K/R/P),硬件設備映射與核心數據結構