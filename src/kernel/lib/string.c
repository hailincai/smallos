#include "types.h"
#include "string.h"
#include "math.h"

/**
 * update string to lowercase
 */
char* k_to_lowercase(char *src)
{
    if (src == NULL) return NULL;

    char *tmp = src;
    while (*tmp != '\0')
    {
        if ('A' <= *tmp && *tmp <= 'Z')
        {
            *tmp = (*tmp - 'A') + 'a';
        }
        tmp++;
    }

    return src;
}

/* * 將字串在第一個分隔符處切斷。
 * s: 輸入字串 (例如 "echo  hello")
 * delimiter: 分隔符 (例如 ' ')
 * 回傳: 指向第二個部分的指標 (例如 "hello")，並將原字串在分隔符處填入 \0
 */
char* k_str_split_first(char *s, char delimiter)
{
    if (s == NULL) return NULL;

    char *p = k_str_find_char(s, delimiter);
    if (p == NULL) return NULL;

    //end the string
    *p = '\0';

    // 3. 跳過後續連續的分隔符 (處理多個空格)
    char *next = p + 1;
    while (*next != '\0' && *next == delimiter) {
        next++;
    }

    // 4. 如果後面全是空格，回傳 NULL，否則回傳參數起始位址
    return (*next == '\0') ? NULL : next;    
}

char* k_str_find_char(const char *s, char c)
{
    if (s == NULL) return NULL;
    while (*s != '\0')
    {
        if (*s == c) 
        {
            return (char *)s;
        }
        s++;
    }

    return NULL;
}

char * k_str_copy_n(char *dest, const char *src, int n)
{
    if (dest == NULL || src == NULL) return dest;

    while (n > 0)
    {
        *dest = *src;
        dest++;
        src++;
        n--;
    }
    *dest = '\0';

    return dest;
}

/* * 將 src 拷貝到 dest
 * 注意：呼叫者必須確保 dest 有足夠的空間
 * 回傳: dest 的起始位址
 */
char* k_str_copy(char *dest, const char *src) 
{
    if (dest == NULL || src == NULL) return dest;

    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
    return dest;
}

int k_str_len(const char s[])
{
    if (s == NULL) return 0;
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

int k_str_has_prefix(const char *s, const char *prefix)
{
    if (s == NULL || prefix == NULL) return 0;
    if (k_str_len(s) < k_str_len(prefix)) return 0;

    while (*prefix != '\0')
    {
        if (*s != *prefix) return 0;
        s++;
        prefix++;
    }

    return 1;
}

int k_str_cmp(char *s1, char *s2)
{
    if (s1 == NULL && s2 == NULL) return 0;
    if (s1 == NULL ) return -1;
    if (s2 == NULL) return 1;

    int str1_len = k_str_len(s1);
    int str2_len = k_str_len(s2);
    int min_len = k_min(str1_len, str2_len);

    for (int i = 0; i <= min_len; i++)
    {
        if (s1[i] > s2[i]) return 1;
        if (s1[i] < s2[i]) return -1;
    }

    return (str1_len == str2_len) ? 0 : ((str1_len == min_len) ? -1 : 1);
}