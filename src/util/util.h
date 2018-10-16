/*
 * Author: Jia Yang
 */

#ifndef JVM_UTIL_H
#define JVM_UTIL_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 * 判断 long_str 是不是以 short_str 结尾。
 */
static bool strend(const char *long_str, const char *short_str)
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

#endif //JVM_UTIL_H
