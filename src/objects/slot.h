/*
 * Author: kayo
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include <cstdint>
#include <cassert>
#include "../jtypes.h"


// 一个slot_t类型必须可以容纳jbool, jbyte, jchar, jshort，jint，jfloat, jref称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef intptr_t slot_t;

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))


#endif //JVM_SLOT_H
