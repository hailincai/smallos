#include "kheap.h"
#include "display.h"

static kheap_header_t *start_node = NULL;
extern page_directory_t* kernel_directory;

kheap_header_t *get_kheap_start()
{
    return start_node;
}

void kheap_coalesce_all() {
    kheap_header_t *current = start_node;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            // 合併後面的塊
            current->size += current->next->size;
            current->next = current->next->next;
            // 注意：合併後不要移動 current，因為下一塊可能還能合併
        } else {
            current = current->next;
        }
    }
}

void init_kheap()
{
    // 1. 為堆區分配初始的 1MB 虛擬空間並映射物理頁
    for (u32 i = 0; i < KHEAP_INITIAL_SIZE; i += PAGE_SIZE)
    {
        // allocate one physical address, 
        // then assoicate the physical address with the virtual address
        u32 phy_addr = (u32)pmm_alloc_page();
        vmm_map(kernel_directory, KHEAP_START + i, phy_addr, VMM_PAGE_PRESENT | VMM_PAGE_RW);
    }

    // 2. 在堆的起始位置建立第一個 Header
    start_node = (kheap_header_t*)KHEAP_START;
    start_node->size = KHEAP_INITIAL_SIZE;
    start_node->is_free = 1;
    start_node->next = NULL;

    kprintf("Kernel Heap initialized at %x, size: 1MB\n", KHEAP_START);
}

void *kmalloc(size_t size)
{
    // 如果size <= 0, return NULL
    if (size == 0) return NULL;

    // 確保不小於最小值
    if (size < KHEAP_MIN_BLOCK_SIZE) {
        size = KHEAP_MIN_BLOCK_SIZE;
    }

    // 確保 4 字節對齊 (x86 下的良好實踐)
    size = (size + 3) & ~3;    

    // 新塊需要的內存包含申請的內存+一個鍊錶頭
    u32 new_block_size = size + sizeof(kheap_header_t);

    u8 retry_count = 0;

find_block:
    {
        kheap_header_t *current = start_node;

        while (current)
        {
            // 這一塊的大小合適，拿來使用
            if (current->is_free && current->size >= new_block_size)
            {
                // 檢測是否需要創建下一塊
                if (current->size > (new_block_size + sizeof(kheap_header_t) + KHEAP_MIN_BLOCK_SIZE))
                {
                    //可以切開
                    kheap_header_t *next_block_header = (kheap_header_t *)((u32)current + new_block_size);
                    next_block_header->is_free = 1;
                    next_block_header->size = current->size - new_block_size;
                    next_block_header->next = current->next;

                    current->size = new_block_size;
                    current->next = next_block_header;
                }

                current->is_free = 0;

                // 返回current + sizeof(header)之後的地址給用戶
                return (void *)((u32)current + sizeof(kheap_header_t));
            }
            
            current = current->next;
        }
    }

    if (retry_count == 0)
    {
        kprintf("Kmalloc: No space, trying to coalesce memory...\n");
        kheap_coalesce_all(); // 執行你寫的全量合併函數
        retry_count++;
        goto find_block;      // --- 使用 goto 跳轉回標號處重試 ---        
    }

    //如果執行到這裡表示heap mem用完了，輸出一個error message, return null
    kprint("Error: kmalloc failed, out of heap memory!\n");
    return NULL;
}

void kfree(void *p) {
    if (!p) return;

    // 1. 找回標頭
    kheap_header_t *header = (kheap_header_t *)((u32)p - sizeof(kheap_header_t));

    // 2. 標記為空閒
    header->is_free = 1;

    // 3. 向後合併 (Merge with next)
    // 只要下一個塊存在且是空閒的，就一直合併
    while (header->next && header->next->is_free) {
        kheap_header_t *next_node = header->next;
        
        // 吞併下一個塊的大小
        header->size += next_node->size;
        
        // 跳過下一個塊，指向「下下個」塊
        header->next = next_node->next;
        
        // 繼續循環，檢查新的 header->next 是否還能合併
    }
}

void print_kheap(char *msg, kheap_debug_ptr_t *list, int list_size) {
    kheap_header_t *current = get_kheap_start();
    kprintf("\n--- Heap Status: %s ---\n", msg);

    while (current) {
        u32 data_start = (u32)current + sizeof(kheap_header_t);
        u32 data_end = data_start + (current->size - sizeof(kheap_header_t));
        
        kprintf("Block [%x]: Size: 0x%x, %s", 
                (u32)current, current->size, current->is_free ? "FREE" : "USED");

        if (!current->is_free && list != NULL) {
            for (int i = 0; i < list_size; i++) {
                u32 target = (u32)list[i].ptr;
                if (target == 0) continue;

                if (target == data_start) {
                    kprintf("  <-- [%s MATCH!]", list[i].label);
                } 
                else if (target > data_start && target < data_end) {
                    kprintf("  <-- [%s INSIDE]", list[i].label);
                }
            }
        }
        kprint("\n");
        current = current->next;
    }
    kprint("--------------------------------------\n");
}