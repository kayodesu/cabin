/*
 * Author: Jia Yang
 */

#ifndef JVM_STRPOOL_H
#define JVM_STRPOOL_H

#include "jstrobj.h"

struct jstrobj* put_str_to_pool(struct classloader *loader, const char *str0);

struct jstrobj* get_str_from_pool(struct classloader *loader, const char *str0);

#endif //JVM_STRPOOL_H
