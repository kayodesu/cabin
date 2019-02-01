/*
 * Author: Jia Yang
 */

#include <wchar.h>
#include <string.h>
#include "encoding.h"
#include "../jvm.h"

jchar* utf8_to_unicode(const const char *str)
{
    size_t len = strlen(str);
    jchar *wstr = vm_malloc(sizeof(jchar)*(len + 1));
    mbstowcs(wstr, str, len);
    wstr[len] = L'\0';
    return wstr;
}

char* unicode_to_utf8(const const jchar arr[], size_t len)
{
    char *str = vm_malloc(sizeof(char)*(len + 1));
    wcstombs(str, arr, len);
    str[len] = '\0';
    return str;
}
