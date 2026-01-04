#!/bin/bash

# 建立必要的目錄
mkdir -p build bin

echo ">>> 編譯核心 (C Code)..."
# -Isrc/kernel 讓編譯器能找到 display.h
gcc -m32 -ffreestanding -fno-pic -fno-stack-protector -c kernel.c -o build/kernel.o

echo ">>> 編譯核心進入點 (ASM)..."
nasm kernel_entry.asm -f elf -o build/kernel_entry.o

echo ">>> 鏈結核心..."
ld -m elf_i386 -o build/kernel.bin -Ttext 0x1000 build/kernel_entry.o build/kernel.o --oformat binary

echo ">>> 編譯 Bootloader..."
# -Isrc/boot 讓 nasm 能找到 %include 的檔案
nasm -f bin boot.asm -o build/bootloader.bin -I.

echo ">>> 製作磁碟鏡像 (os-image.bin)..."
# 合併 Bootloader 與 Kernel
cat build/bootloader.bin build/kernel.bin > bin/os-image.bin

# 確保鏡像至少有 20 個磁區 (約 10KB)，避免 BIOS 讀取錯誤
dd if=/dev/zero bs=512 count=20 >> bin/os-image.bin
# 重新截取前 20 個磁區，保持檔案精簡固定
truncate -s 10240 bin/os-image.bin

echo ">>> 編譯完成！鏡像檔位於 bin/os-image.bin"

# 詢問是否執行 QEMU
read -p "是否立即執行 QEMU? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    qemu-system-i386 -drive format=raw,file=bin/os-image.bin
fi