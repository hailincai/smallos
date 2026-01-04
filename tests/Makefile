# tests/Makefile

HOST_CC = gcc
# 加入 -fno-builtin 防止主機 C 庫函式干擾自定義實現
CFLAGS = -g -Wall -fno-builtin -I../src/kernel/lib -DHOST_TESTING

# 目錄定義
SRC_LIB_DIR = ../src/kernel/lib
TEST_LIB_DIR = kernel/lib
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/lib_objs

# 1. 獲取所有內核庫源文件並定義對應的 Object 檔案
LIB_SRCS = $(wildcard $(SRC_LIB_DIR)/*.c)
LIB_OBJS = $(patsubst $(SRC_LIB_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

# 2. 獲取所有測試源文件
TEST_SRCS = $(wildcard $(TEST_LIB_DIR)/test_*.c)
# 轉換路徑：將 tests/kernel/lib/test_string.c 轉為 build/test_string
TEST_BINS = $(patsubst $(TEST_LIB_DIR)/test_%.c, $(BUILD_DIR)/test_%, $(TEST_SRCS))
TEST_NAMES = $(patsubst $(TEST_LIB_DIR)/test_%.c, %, $(TEST_SRCS))

# 防止 make 自動刪除 .o 檔案
.PRECIOUS: $(OBJ_DIR)/%.o

# --- 目標規則 ---

.PHONY: test_all clean $(TEST_NAMES)

# 執行所有測試
test_all: $(TEST_BINS)
	@echo "======================================="
	@echo "   Running All Kernel Library Tests    "
	@echo "======================================="
	@for bin in $(TEST_BINS); do \
		echo "Executing $$bin..."; \
		./$$bin || exit 1; \
		echo "---------------------------------------"; \
	done
	@echo "All tests passed successfully!"

# 規則 A：編譯內核庫的所有 .c 檔案為 .o (主機環境)
$(OBJ_DIR)/%.o: $(SRC_LIB_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(HOST_CC) $(CFLAGS) -c $< -o $@

# 規則 B：編譯測試程序並連結「所有」內核庫 Object
$(BUILD_DIR)/test_%: $(TEST_LIB_DIR)/test_%.c $(LIB_OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $@ with all library objects..."
	$(HOST_CC) $(CFLAGS) $^ -o $@

# 單獨執行命令 (例如: make string)
# 注意：這裡對應的是 test_string.c，所以輸入 make string 會執行 build/test_string
$(TEST_NAMES): %: $(BUILD_DIR)/test_%
	@echo "Executing $<..."
	@./$<

clean:
	rm -rf $(BUILD_DIR)