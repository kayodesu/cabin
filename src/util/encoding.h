/*
 * Author: Jia Yang
 */

#ifndef JVM_ENCODING_H
#define JVM_ENCODING_H

#include <stddef.h>
#include "../jtypes.h"

void* decode_mutf8(const uint8_t *src_bytes, size_t len, char dest[len + 1]);

jchar* utf8_to_unicode(const const char *str);

char* unicode_to_utf8(const const jchar *wstr);

#endif //JVM_ENCODING_H
