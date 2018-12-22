/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "../jvm.h"

bool vm_strend(const char *long_str, const char *short_str)
{
    assert(long_str != NULL);
    assert(short_str != NULL);

    size_t llen = strlen(long_str);
    size_t slen = strlen(short_str);
    if (slen > llen)
        return false;

    const char *p = long_str + llen - slen;
    return strcmp(p, short_str) == 0;
}

void vm_strrpl(char str[static 1], char s, char d)
{
    for (char *p = str; *p != 0; p++) {
        if (*p == s) {
            *p = d;
        }
    }
}

char* vm_strdup(const char *s)
{
    assert(s != NULL);

    char *ns = malloc(sizeof(char) * strlen(s) + 1);
    CHECK_MALLOC_RESULT(ns);
    strcpy(ns, s);
    return ns;
}
