# ==========================================
# GeminiOS 核心編譯控制檔
# ==========================================

# 1. 工具定義
CC = gcc
LD = ld
NASM = nasm

# 2. 編譯選項
# -m32: 產生 32 位元代碼
# -ffreestanding: 不依賴標準庫
# -fno-pic: 禁用位置無關代碼 (用於內核)
# -fno-stack-protector: 禁用堆疊保護，避免連結到不存在的庫
# -fno-builtin: 確保使用我們自定義的 string 函式 (如 str_len)
CFLAGS = -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Wall -Wextra \
         -Isrc/kernel/cpu -Isrc/kernel/drivers -Isrc/kernel/lib -Isrc/kernel/mm -Isrc/kernel

# 3. 目錄與檔案定義
SRC_DIR = src/kernel
BUILD_DIR = build

# 自動搜尋所有的 C 文件與標頭檔
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
HEADERS = $(shell find $(SRC_DIR) -name "*.h")

# 這裡非常重要：從 C_SOURCES 中過濾掉核心入口 kernel.c
# 因為我們要在鏈結時手動控制它的排序，避免編譯器隨機排序
OTHER_C_SOURCES = $(filter-out $(SRC_DIR)/kernel.c, $(C_SOURCES))

# 將路徑映射到 build/ 目錄
OBJ = $(OTHER_C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 手動定義核心的「絕對順序」物件
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/boot/kernel_entry.o
KERNEL_MAIN_OBJ = $(BUILD_DIR)/kernel.o
INTERRUPT_OBJ = $(BUILD_DIR)/cpu/interrupt.o

# ==========================================
# 4. 最終目標規則
# ==========================================

all: os-image.bin

# 製作 OS 鏡像 (將 Bootloader 與 Kernel 拼接)
os-image.bin: $(BUILD_DIR)/boot/bootsect.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@
	@# 獲取當前總大小並對齊到 512 的倍數
	@CUR_SIZE=$$(stat -c %s $@); \
	 ALIGNED_SIZE=$$(( ((CUR_SIZE + 511) / 512) * 512 )); \
	 truncate -s $$ALIGNED_SIZE $@
	@echo "OS Image created and aligned to $$(( $$(stat -c %s $@) / 512 )) sectors."

# 5. 鏈結核心 (關鍵中的關鍵)
# 順序必須是：Entry -> Main -> Interrupt -> Others
$(BUILD_DIR)/kernel.bin: $(KERNEL_ENTRY_OBJ) $(KERNEL_MAIN_OBJ) $(INTERRUPT_OBJ) $(OBJ)
	@echo "Linking Kernel Binary..."
	$(LD) -m elf_i386 -o $@ -T src/link.ld $^ --oformat binary
	@echo "Kernel size: $$(stat -c %s $@) bytes"

# ==========================================
# 6. 編譯規則
# ==========================================

# 編譯主核心 C 文件 (kernel.c)
$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 通用的 C 文件編譯規則
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 編譯核心入口 (kernel_entry.asm)
$(BUILD_DIR)/boot/kernel_entry.o: src/boot/kernel_entry.asm
	@mkdir -p $(dir $@)
	$(NASM) $< -f elf32 -o $@

# 編譯中斷跳板 (interrupt.asm)
$(BUILD_DIR)/cpu/interrupt.o: src/kernel/cpu/interrupt.asm
	@mkdir -p $(dir $@)
	$(NASM) $< -f elf32 -o $@

# 編譯 Bootloader (直接輸出為純二進制 bin)
# 增加了对kernel.bin的依賴，所以可以計算實際的sector大小並傳入匯編
$(BUILD_DIR)/boot/bootsect.bin: src/boot/boot.asm $(BUILD_DIR)/kernel.bin
	@# 計算 kernel 所需扇區
	$(eval K_SECTORS=$(shell echo $$(( ($$(stat -c %s $(BUILD_DIR)/kernel.bin) + 511) / 512 ))))
	@# 使用 -D 參數定義常量傳給 NASM
	nasm -f bin $< -I src/boot/ -D KERNEL_SECTORS=$(K_SECTORS) -o $@

# ==========================================
# 7. 管理指令
# ==========================================

clean:
	rm -rf build/ os-image.bin

run: os-image.bin
	qemu-system-i386 -drive format=raw,file=os-image.bin -d int,cpu_reset -no-reboot