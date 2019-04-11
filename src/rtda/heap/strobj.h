/*
 * Author: Jia Yang
 */

#ifndef JVM_STROBJ_H
#define JVM_STROBJ_H

#include "object.h"

Object* strobj_create(const char *str);
const char* strobj_value(Object *so);

#endif //JVM_STROBJ_H
