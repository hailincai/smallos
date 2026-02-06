#ifndef UTIL_H
#define UTIL_H

// 計算靜態數組的長度
// 注意：這只能用於真正的數組，不能用於已經退化為指針的參數
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
#endif