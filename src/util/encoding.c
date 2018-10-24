/*
 * Author: Jia Yang
 */

#include <afxres.h>
#include <wchar.h>
#include "encoding.h"

void* decode_mutf8(uint8_t *src_bytes, size_t len, char dest[len + 1])
{
    // todo 此函数的实现是错的，先这么做
    memcpy(dest, src_bytes, len);
    dest[len] = 0;
    return dest;
}

jchar* utf8_to_unicode(const char *str)
{
    size_t len = strlen(str);
    VM_MALLOCS(jchar, len + 1, wstr);
    mbstowcs(wstr, str, len);
    wstr[len] = L'\0';
    return wstr;
}

char* unicode_to_utf8(const jchar *wstr)
{
    size_t len = wcslen(wstr);
    VM_MALLOCS(char, len + 1, str);
    wcstombs(str, wstr, len);
    str[len] = '\0';
    return str;
}