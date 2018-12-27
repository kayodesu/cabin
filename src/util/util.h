/*
 * Author: Jia Yang
 */

#ifndef JVM_UTIL_H
#define JVM_UTIL_H

#include <stdbool.h>

/*
 * 判断 long_str 是不是以 short_str 结尾。
 */
bool vm_strend(const char *long_str, const char *short_str);

/*
 * string replace
 * 将字符串 @str 中所有的字符 @s 替换成 @d
 */
void vm_strrpl(char str[static 1], char s, char d);

char* vm_strdup(const char *s);

#endif //JVM_UTIL_H
