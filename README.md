## 內核基礎庫成員

為了確保內核的自足性（Self-sufficiency），我們實作了以下三個模組：

- **`lib/types.h`**: 定義固定長度的類型（如 `u32`）與 `NULL`。這是確保 C 代碼在 32 位元環境下地址計算正確的基石。
- **`lib/string.c`**: 實作 `k_str_len` , `k_str_cmp`, `k_str_has_prefix`, `k_str_copy`, `k_str_copy_n`, `k_str_find_char`, `k_str_split_first`與 `k_to_lowercase` 等字串函式。
- **`lib/math.c`**: 提供 `k_min` 等基礎數學運算，用於處理顯示邊界。
- **`lib/util.c`**: **Debug 的核心工具**。負責將整數格式化為「十進位字串」與「十六進位字串」（例如將地址 `0x1000` 轉為文字）。
