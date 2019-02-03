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
const char* find_hashed_utf8(const char *utf8, bool add_if_absent);

#define new_utf8(s) find_hashed_utf8(s, true)
#define find_utf8(s) find_hashed_utf8(s, false)
#define utf8_equals(s, t) (((s) == (t)) || (strcmp(s, t) == 0))

#endif //JVM_UTF8_H
