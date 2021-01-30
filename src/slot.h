#ifndef KAYO_SLOT_H
#define KAYO_SLOT_H

#include <cassert>
#include <typeinfo>
#include "jvmstd.h"

// 一个slot_t类型必须可以容纳 jbool, jbyte, jchar, jshort，jint，jfloat, jref 称为类型一
// jlong, jdouble 称为类型二，占两个slot
typedef intptr_t slot_t;

static_assert(sizeof(slot_t) >= sizeof(jbool));
static_assert(sizeof(slot_t) >= sizeof(jbyte));
static_assert(sizeof(slot_t) >= sizeof(jchar));
static_assert(sizeof(slot_t) >= sizeof(jshort));
static_assert(sizeof(slot_t) >= sizeof(jint));
static_assert(sizeof(slot_t) >= sizeof(jfloat));
static_assert(sizeof(slot_t) >= sizeof(jref));
static_assert(2*sizeof(slot_t) >= sizeof(jlong));
static_assert(2*sizeof(slot_t) >= sizeof(jdouble));

namespace slot {
    /* build slot */

    slot_t islot(jint v);
    slot_t fslot(jfloat v);
    slot_t rslot(jref v);

    /* setter */

    void setInt(slot_t *slots, jint v);
    void setByte(slot_t *slots, jbyte v);
    void setBool(slot_t *slots, jbool v);
    void setChar(slot_t *slots, jchar v);
    void setShort(slot_t *slots, jshort v);
    void setFloat(slot_t *slots, jfloat v);
    void setLong(slot_t *slots, jlong v);
    void setDouble(slot_t *slots, jdouble v);
    void setRef(slot_t *slots, jref v);

    /* getter */

    jint getInt(const slot_t *slots);
    jbyte getByte(const slot_t *slots);
    jbool getBool(const slot_t *slots);
    jchar getChar(const slot_t *slots);
    jshort getShort(const slot_t *slots);
    jfloat getFloat(const slot_t *slots);
    jlong getLong(const slot_t *slots);
    jdouble getDouble(const slot_t *slots);
    jref getRef(const slot_t *slots);
}

#endif //KAYO_SLOT_H
