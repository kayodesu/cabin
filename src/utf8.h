/*
 * Author: Jia Yang
 */

#ifndef JVM_UTF8_H
#define JVM_UTF8_H

#include <stdbool.h>

void utf8_init();
int utf8_hash(const char *utf8);

/*
 * if @utf8 is present, return hashed utf8.
 *
 * if @utf8 is absent and @add_if_absent is false return NULL,
 * otherwise put @utf8 to hashset and return @utf8.
 */
char* find_hashed_utf8(char *utf8, bool add_if_absent);

#define new_utf8(s) find_hashed_utf8(s, true)
#define find_utf8(s) find_hashed_utf8(s, false)

#endif //JVM_UTF8_H
