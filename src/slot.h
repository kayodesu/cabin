/*
 * Author: Yo Ka
 */

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

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

namespace slot {
    /* build slot */

    static inline slot_t rslot(jref v)
    {
        slot_t s;
        RSLOT(&s) = v;
        return s;
    }

    static inline slot_t islot(jint v)
    {
        slot_t s;
        ISLOT(&s) = v;
        return s;
    }

    static inline slot_t fslot(jfloat v)
    {
        slot_t s;
        FSLOT(&s) = v;
        return s;
    }

    static inline slot_t lslot(jlong v)
    {
        slot_t s;
        LSLOT(&s) = v;
        return s;
    }

    static inline slot_t dslot(jdouble v)
    {
        slot_t s;
        DSLOT(&s) = v;
        return s;
    }

    /* setter */

    static inline void setInt(slot_t *slots, jint v)
    {
        assert(slots != nullptr);
        ISLOT(slots) = v;
    }

    static inline void setByte(slot_t *slots, jbyte v)
    {
        assert(slots != nullptr);
        setInt(slots, v);
    }

    static inline void setBool(slot_t *slots, jbool v)
    {
        assert(slots != nullptr);
        setInt(slots, v);
    }

    static inline void setChar(slot_t *slots, jchar v)
    {
        assert(slots != nullptr);
        setInt(slots, v);
    }

    static inline void setShort(slot_t *slots, jshort v)
    {
        assert(slots != nullptr);
        setInt(slots, v);
    }

    static inline void setFloat(slot_t *slots, jfloat v)
    {
        assert(slots != nullptr);
        FSLOT(slots) = v;
    }

    static inline void setLong(slot_t *slots, jlong v)
    {
        assert(slots != nullptr);
        LSLOT(slots) = v;
    }

    static inline void setDouble(slot_t *slots, jdouble v)
    {
        assert(slots != nullptr);
        DSLOT(slots) = v;
    }

    static inline void setRef(slot_t *slots, jref v)
    {
        assert(slots != nullptr);
        RSLOT(slots) = v;
    }

    /* getter */

    static inline jint getInt(slot_t *slots)       { assert(slots != nullptr); return ISLOT(slots); }
    static inline jbyte getByte(slot_t *slots)     { assert(slots != nullptr); return jint2jbyte(getInt(slots)); }
    static inline jbool getBool(slot_t *slots)     { assert(slots != nullptr); return jint2jbool(getInt(slots)); }
    static inline jchar getChar(slot_t *slots)     { assert(slots != nullptr); return jint2jchar(getInt(slots)); }
    static inline jshort getShort(slot_t *slots)   { assert(slots != nullptr); return jint2jshort(getInt(slots)); }
    static inline jfloat getFloat(slot_t *slots)   { assert(slots != nullptr); return FSLOT(slots); }
    static inline jlong getLong(slot_t *slots)     { assert(slots != nullptr); return LSLOT(slots); }
    static inline jdouble getDouble(slot_t *slots) { assert(slots != nullptr); return DSLOT(slots); }
    template <typename T = Object>
    static inline T *getRef(slot_t *slots)         { assert(slots != nullptr); return (T *) RSLOT(slots); }
}

#endif //KAYO_SLOT_H
