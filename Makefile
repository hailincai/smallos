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
         -Isrc/kernel/cpu -Isrc/kernel/drivers -Isrc/kernel/lib -Isrc/kernel

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

# ==========================================
# 4. 最終目標規則
# ==========================================

all: os-image.bin

# 製作 OS 鏡像 (將 Bootloader 與 Kernel 拼接)
os-image.bin: $(BUILD_DIR)/boot/bootsect.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@
	# 這裡補齊磁區：如果你的核心變大了，請確保 dd 的 count 足夠大
	# 目前設定補齊到約 32KB (64 個磁區)
	dd if=/dev/zero bs=512 count=64 >> $@ 2>/dev/null
	# 強制截斷到 32768 bytes (64 sectors * 512 bytes)
	truncate -s 32768 $@

# 5. 鏈結核心 (關鍵中的關鍵)
# 順序必須是：Entry -> Main -> Interrupt -> Others
$(BUILD_DIR)/kernel.bin: $(KERNEL_ENTRY_OBJ) $(KERNEL_MAIN_OBJ) $(OBJ)
	@echo "Linking Kernel Binary..."
	$(LD) -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary
	@echo "Kernel size: $$(stat -c %s $@) bytes"
	@echo "Sectors needed: $$(( ($$(stat -c %s $@) + 511) / 512 ))"	

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
$(BUILD_DIR)/boot/bootsect.bin: src/boot/boot.asm
	@mkdir -p $(dir $@)
	$(NASM) $< -f bin -I src/boot/ -o $@

# ==========================================
# 7. 管理指令
# ==========================================

clean:
	rm -rf build/ os-image.bin

run: os-image.bin
	qemu-system-i386 -drive format=raw,file=os-image.bin