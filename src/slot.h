/*
 * Author: Jia Yang
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include <stdint.h>
#include "jtypes.h"

// 一个slot_t类型必须可以容纳jbool, jbyte, jchar, jshort，jint，jfloat, jref称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef uintptr_t slot_t;

#define ISLOT(__slot) (* (jint *) (__slot))
#define FSLOT(__slot) (* (jfloat *) (__slot))
#define LSLOT(__slot) (* (jlong *) (__slot))
#define DSLOT(__slot) (* (jdouble *) (__slot))
#define RSLOT(__slot) (* (jref *) (__slot))

#endif //JVM_SLOT_H
