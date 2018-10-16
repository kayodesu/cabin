/*
 * Author: Jia Yang
 */

#ifndef JVM_ENCODING_H
#define JVM_ENCODING_H

#include <stdint.h>
#include <limits.h>
#include <string.h>

static inline void* decode_mutf8(uint8_t *src_bytes, size_t len, char dest[len + 1]) {
    // todo 此函数的实现是错的，先这么做
    memcpy(dest, src_bytes, len);
    dest[len] = 0;
    return dest;
}

static inline jchar* utf8_to_unicode(const char *src)
{
    // todo
    return NULL;
}

static inline char * unicode_to_utf8(const jchar* src)
{
    // todo
    return NULL;
}

#endif //JVM_ENCODING_H
