#ifndef STRING_H
#define STRING_H

#include "types.h"

int k_str_len(const char s[]);
int k_str_cmp(char *s1, char *s2);
int k_str_has_prefix(const char *s, const char *prefix);
char* k_str_copy(char *dest, const char *src);
char * k_str_copy_n(char *dest, const char *src, int n);
char* k_str_find_char(const char *s, char c);
char* k_str_split_first(char *s, char delimiter);
char * k_to_lowercase(char *src);
void *k_mem_set(void *dest, u8 val, u32 len);
void *k_mem_copy(void *dest, const void *src, u32 len);
#endif