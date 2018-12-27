/*
 * Author: Jia Yang
 */

#ifndef JVM_STROBJ_H
#define JVM_STROBJ_H

#include "object.h"

struct object* strobj_create(const char *str);
const char* strobj_value(struct object *so);

#endif //JVM_STROBJ_H
