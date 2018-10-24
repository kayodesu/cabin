/*
 * Author: Jia Yang
 */

#ifndef JVM_ENCODING_H
#define JVM_ENCODING_H

#include "../jtypes.h"

void* decode_mutf8(uint8_t *src_bytes, size_t len, char dest[len + 1]);

jchar* utf8_to_unicode(const char *str);

char* unicode_to_utf8(const jchar *wstr);

#endif //JVM_ENCODING_H
