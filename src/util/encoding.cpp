/*
 * Author: Jia Yang
 */

#include <cstring>
#include "encoding.h"
#include "../jvm.h"

jchar *utf8_to_unicode(const char *str)
{
    size_t len = strlen(str);
    jchar *wstr = new jchar[len + 1];//vm_malloc(sizeof(jchar)*(len + 1));
    mbstowcs((wchar_t *) wstr, str, len);
    wstr[len] = L'\0';
    return wstr;
}

char *unicode_to_utf8(const jchar arr[], size_t len)
{
    char *str = new char[len + 1];//vm_malloc(sizeof(char)*(len + 1));
    wcstombs(str, (wchar_t *) arr, len);
    str[len] = '\0';
    return str;
}
