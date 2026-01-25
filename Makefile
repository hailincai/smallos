# ==========================================
# GeminiOS 核心編譯控制檔 (支持用戶態分離)
# ==========================================

# 1. 工具定義
CC = gcc
LD = ld
NASM = nasm

# 2. 編譯選項
CFLAGS = -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Wall -Wextra \
         -Isrc/kernel/cpu -Isrc/kernel/drivers -Isrc/kernel/lib -Isrc/kernel/mm -Isrc/kernel -Isrc/include
USER_CFLAGS = -m32 -ffreestanding -fno-pic -fno-stack-protector -fno-builtin -Wall -Wextra \
         -Isrc/user/include

# 3. 目錄與檔案定義
SRC_DIR = src/kernel
USER_SRC_DIR = src/user
BUILD_DIR = build
USER_BUILD_DIR = $(BUILD_DIR)/user

# 自動搜尋所有的 C 文件與標頭檔
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
HEADERS = $(shell find $(SRC_DIR) -name "*.h")

# 過濾核心入口
OTHER_C_SOURCES = $(filter-out $(SRC_DIR)/kernel.c, $(C_SOURCES))
OBJ = $(OTHER_C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 物件檔案定義
KERNEL_ENTRY_OBJ = $(BUILD_DIR)/boot/kernel_entry.o
KERNEL_MAIN_OBJ = $(BUILD_DIR)/kernel.o
INTERRUPT_OBJ = $(BUILD_DIR)/cpu/interrupt.o
# 新增：包裝用戶二進位檔的物件
USER_BIN_OBJ = $(BUILD_DIR)/kernel/user_bin.o

# ==========================================
# 4. 最終目標規則
# ==========================================

all: os-image.bin

# 製作 OS 鏡像
os-image.bin: $(BUILD_DIR)/boot/bootsect.bin $(BUILD_DIR)/kernel.bin
	cat $^ > $@
	@CUR_SIZE=$$(stat -c %s $@); \
	 ALIGNED_SIZE=$$(( ((CUR_SIZE + 511) / 512) * 512 )); \
	 truncate -s $$ALIGNED_SIZE $@
	@echo "OS Image created."

# 5. 鏈結核心 (加入 USER_BIN_OBJ)
$(BUILD_DIR)/kernel.bin: $(KERNEL_ENTRY_OBJ) $(KERNEL_MAIN_OBJ) $(INTERRUPT_OBJ) $(USER_BIN_OBJ) $(OBJ)
	@echo "Linking Kernel Binary..."
	$(LD) -m elf_i386 -o $@ -T src/link.ld $^ --oformat binary

# ==========================================
# 6. 用戶態編譯規則 (新增)
# ==========================================

# 生成用戶態二進位檔
$(BUILD_DIR)/init.bin: $(USER_SRC_DIR)/start.asm $(USER_SRC_DIR)/main.c
	@mkdir -p $(USER_BUILD_DIR)
	@echo "Compiling User Program..."
	$(NASM) $(USER_SRC_DIR)/start.asm -f elf32 -o $(USER_BUILD_DIR)/start.o
	$(CC) -m32 -c $(USER_SRC_DIR)/main.c -o $(USER_BUILD_DIR)/main.o $(USER_CFLAGS) -fno-pic
	$(LD) -m elf_i386 -T $(USER_SRC_DIR)/user.ld $(USER_BUILD_DIR)/start.o $(USER_BUILD_DIR)/main.o -o $@

# 將 init.bin 透過 nasm 包裝成內核物件
$(USER_BIN_OBJ): src/kernel/user_bin.asm $(BUILD_DIR)/init.bin
	@mkdir -p $(dir $@)
	@# 使用 -I 指向 build 目錄，讓 incbin 能找到 init.bin
	$(NASM) $< -f elf32 -I$(BUILD_DIR)/ -o $@

# ==========================================
# 7. 內核編譯規則
# ==========================================

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/boot/kernel_entry.o: src/boot/kernel_entry.asm
	@mkdir -p $(dir $@)
	$(NASM) $< -f elf32 -o $@

$(BUILD_DIR)/cpu/interrupt.o: src/kernel/cpu/interrupt.asm
	@mkdir -p $(dir $@)
	$(NASM) $< -f elf32 -o $@

$(BUILD_DIR)/boot/bootsect.bin: src/boot/boot.asm $(BUILD_DIR)/kernel.bin
	$(eval K_SECTORS=$(shell echo $$(( ($$(stat -c %s $(BUILD_DIR)/kernel.bin) + 511) / 512 ))))
	$(NASM) -f bin $< -I src/boot/ -D KERNEL_SECTORS=$(K_SECTORS) -o $@

# ==========================================
# 8. 管理指令
# ==========================================

clean:
	rm -rf build/ os-image.bin init.bin

run: os-image.bin
	qemu-system-i386 -drive format=raw,file=os-image.bin -d int,cpu_reset -no-reboot