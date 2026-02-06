# 🚀 SmallOS

SmallOS 是一個從零開始（From Scratch）開發的 x86 作業系統實踐項目。這個項目的目標不僅是啟動內核，而是要完整實踐一個現代作業系統的核心組件，包括中斷處理、記憶體管理、多工調度與檔案系統。

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Language: Assembly](https://img.shields.io/badge/Language-Assembly-red.svg)](https://en.wikipedia.org/wiki/Assembly_language)

---

## 📖 專案紀錄 (Development Journey)
本專案的詳細開發心得與技術細節同步更新於我的 Notion 網站：
👉 [**SmallOS 開發挑戰：從零打造 x86 作業系統**](https://smallos.notion.site/)

---

## 🛠️ 目前功能與進度 (Current Milestones)
由於後面的內容難度越來越高，可能會經過好多天，才能完成一個功能，所以更新速度可能會變慢。

- [x] **Day 1**: 搭建 WSL2 + x86 編譯環境
- [x] **Day 2**: 撰寫 512B 引導扇區 (Boot Sector)
- [x] **Day 3**: 實作 16-bit 彙編字串列印函式
- [x] **Day 4**: 實現磁碟加載 (Disk Loading) 突破空間限制
- [x] **Day 5**: 設置 GDT 並切換至 32-bit 保護模式 (Protected Mode)
- [x] **Day 6**: 啟動第一個 C 語言內核 (C Kernel)
- [x] **Day 7**: 工程化架構重組與 Makefile 自動化編譯
- [x] **Day 8**: 內核基礎設施 —— 數據類型、工具庫與離線測試
- [x] **Day 09**: 核心筆記：從 VGA 輸出到鍵盤中斷全鏈路實作
- [x] **Day 10**：互動的起點 —— 增強鍵盤驅動與 Shell 實作
- [x] **Day 11**：時間的律動 —— PIT 定時器與 CMOS 即時鐘
- [x] **Day 12**：邁向虛擬記憶體 —— 分頁機制 (Paging) 實作
- [x] **Day 13**：跨越 3G 邊界 —— Higher Half Kernel 與地址重定位
- [x] **Day 14**：內核的完全體 —— GDT 重定位與物理記憶體管理 (PMM)
- [x] **Day 15**：構建記憶體秩序 —— 虛擬記憶體管理系統
- [x] **Day 16**：內核動態內存管理 —— kmalloc 與 kfree 的誕生
- [x] **Day 17**：權限隔離的起點 —— 進入用戶態 (User Mode)
- [x] **Day 18**：打破隔離的橋樑 —— 系統調用 (System Call) 的實作
- [x] **Day 19**: 重構：優化內核目錄架構並實作 sys_exit 系統調用

### ⏳ 即將到來 (Coming Soon)
- [ ] 物理與虛擬記憶體管理 (Paging)
- [ ] 搶佔式多工調度 (Scheduler)
- [ ] 基礎驅動 (Keyboard, Timer)

---

## 🚀 如何運行 (Getting Started)

### 前置準備
你需要安裝以下工具（建議在 Ubuntu/WSL2 環境下）：
```bash
sudo apt update
sudo apt install -y nasm qemu-system-x86 build-essential gdb















