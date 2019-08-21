/*
 * Author: kayo
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include <cstdint>
#include "jtypes.h"

#if 0
enum SlotType {
    SLOT_TYPE_JBYTE,
    SLOT_TYPE_JBOOL,
    SLOT_TYPE_JCHAR,
    SLOT_TYPE_JSHORT,
    SLOT_TYPE_JINT,
    SLOT_TYPE_JFLOAT,
    SLOT_TYPE_JREF,
    SLOT_TYPE_JLONG_HIGHT,
    SLOT_TYPE_JLONG_LOW,
    SLOT_TYPE_JDOUBLE_HIGHT,
    SLOT_TYPE_JDOUBLE_LOW
};

struct Slot {//jbyte jbool  jchar  jshort  jint jlong jfloat jdouble jref
    SlotType type;
    union {
        jbyte b;
        jbool z;
        jchar c;
        jshort s;
        jint i;
        jfloat f;
        jref r;
    } v;
};
#endif

// 一个slot_t类型必须可以容纳jbool, jbyte, jchar, jshort，jint，jfloat, jref称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef uintptr_t slot_t;

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

#endif //JVM_SLOT_H
