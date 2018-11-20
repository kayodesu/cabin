/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <string.h>
#include "util.h"

bool strend(const char *long_str, const char *short_str)
{
    if (long_str == NULL || short_str == NULL)
        return false;

    size_t llen = strlen(long_str);
    size_t slen = strlen(short_str);
    if (slen > llen)
        return false;

    const char *p = long_str + llen - slen;
    return strcmp(p, short_str) == 0;
}