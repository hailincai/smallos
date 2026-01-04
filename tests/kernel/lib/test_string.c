#include <stdio.h>
#include <assert.h>
#include "string.h"

int main()
{
    printf("Testing strlen function....\n");
    assert(k_str_len(NULL) == 0);
    assert(k_str_len("a") == 1);
    assert(k_str_len("ab") == 2);

    printf("Testing strcmp function...\n");
    assert(k_str_cmp(NULL, NULL) == 0);
    assert(k_str_cmp(NULL, "a") == -1);
    assert(k_str_cmp("a", NULL) == 1);
    assert(k_str_cmp("ab", "ab") == 0);
    assert(k_str_cmp("abc", "ab") == 1);
    assert(k_str_cmp("ab", "abc") == -1);
    assert(k_str_cmp("ab", "bc") == -1);
    assert(k_str_cmp("bc", "ab") == 1);   
    
    printf("Test k_str_has_prefix....\n");
    assert(k_str_has_prefix(NULL, "a") == 0);
    assert(k_str_has_prefix("a", NULL) == 0);
    assert(k_str_has_prefix("a", "ab") == 0);
    assert(k_str_has_prefix("abc", "ab") == 1);
    assert(k_str_has_prefix("bbc", "ab") == 0);

    printf("Test k_str_copy...\n");
    char dest[10];
    assert(k_str_copy(dest, NULL) == dest);
    assert(k_str_cmp("ab", k_str_copy(dest, "ab")) == 1);
    assert(k_str_cmp("abc", k_str_copy(dest, "abc")) == 1);
    assert(k_str_cmp("a", k_str_copy(dest, "a")) == 1);

    printf("Test k_str_copy_n...\n");
    assert(k_str_copy_n(dest, NULL, 0) == dest);
    assert(k_str_cmp("ab", k_str_copy_n(dest, "ab", 2)) == 1);
    assert(k_str_cmp("abc", k_str_copy_n(dest, "abc", 3)) == 1);
    assert(k_str_cmp("a", k_str_copy_n(dest, "abc", 1)) == 1);     

    printf("Test k_str_find_char...\n");
    assert(k_str_find_char(NULL, 'a') == NULL);
    assert(k_str_find_char("bcd", 'a') == NULL);
    char *tmp = k_str_find_char("bcd", 'c');
    assert(k_str_cmp("cd", tmp) == 0);

    printf("Test k_str_split_first...\n");
    tmp = k_str_split_first(NULL, ' ');
    assert(tmp == NULL);

    k_str_copy(dest, "abc");
    tmp = k_str_split_first("abc", ' ');
    assert(tmp == NULL);
    assert(k_str_cmp("abc", dest) == 0);

    k_str_copy(dest, "ab c");
    tmp = k_str_split_first(dest, ' ');
    assert(k_str_cmp(tmp, "c") == 0);
    assert(k_str_cmp(dest, "ab") == 0);

    k_str_copy(dest, "ab  c");
    tmp = k_str_split_first(dest, ' ');
    assert(k_str_cmp(tmp, "c") == 0);
    assert(k_str_cmp(dest, "ab") == 0);

    k_str_copy(dest, "ab ");
    tmp = k_str_split_first(dest, ' ');
    assert(tmp == NULL);
    assert(k_str_cmp(dest, "ab") == 0);

    printf("Test k_to_lowercase...\n");
    k_str_copy(dest, "ABC");
    assert(k_str_cmp("abc", k_to_lowercase(dest)) == 0);
    k_str_copy(dest, "Abc");
    assert(k_str_cmp("abc", k_to_lowercase(dest)) == 0);
    k_str_copy(dest, "A123");
    assert(k_str_cmp("a123", k_to_lowercase(dest)) == 0);
    assert(k_to_lowercase(NULL) == NULL);

    return 0;
}