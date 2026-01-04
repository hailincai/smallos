#ifndef STRING_H
#define STRING_H
int k_str_len(const char s[]);
int k_str_cmp(char *s1, char *s2);
int k_str_has_prefix(const char *s, const char *prefix);
char* k_str_copy(char *dest, const char *src);
char * k_str_copy_n(char *dest, const char *src, int n);
char* k_str_find_char(const char *s, char c);
char* k_str_split_first(char *s, char delimiter);
char * k_to_lowercase(char *src);
#endif