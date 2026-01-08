#ifndef MEM_H
#define MEM_H
#include "types.h"

#define ENTRIES_COUNT   1024
#define PAGE_SIZE       4096

typedef struct {
    u32 present     :1; //存在位元
    u32 rw          :1; //讀寫位元
    u32 user        :1; //用戶位元
    u32 accessed    :1; //是否被存取過
    u32 dirty       :1; //是否被寫入過（僅PT）
    u32 reserved    :7; //保留位元（未被使用）
    u32 frame_addr  :20;//物理地址高20位，不需要低12位，因為4KB對齊的關係
} page_entry_t;

extern u32 page_directory[ENTRIES_COUNT];
extern u32 first_page_table[ENTRIES_COUNT];

void init_paging();
void enable_paging();
#endif