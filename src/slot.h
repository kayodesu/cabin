/*
 * Author: kayo
 */

#ifndef KAYO_SLOT_H
#define KAYO_SLOT_H

#include <typeinfo>
#include "vmdef.h"
#include "kayo.h"

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
static_assert(2 * sizeof(slot_t) >= sizeof(jlong));
static_assert(2 * sizeof(slot_t) >= sizeof(jdouble));

#define ISLOT(slot_point) (* (jint *) (slot_point))
#define FSLOT(slot_point) (* (jfloat *) (slot_point))
#define LSLOT(slot_point) (* (jlong *) (slot_point))
#define DSLOT(slot_point) (* (jdouble *) (slot_point))
#define RSLOT(slot_point) (* (jref *) (slot_point))

namespace slot {
    // build slot
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

//    // get value
//    template<typename T = slot_t>
//    void  getValue(const slot_t *s, T &value)
//    {
//        if (typeid(T) == typeid(slot_t))
//            value = *s;
//        if (typeid(T) == typeid(jbyte))
//            value = jint2jbyte(ISLOT(s));
//        if (typeid(T) == typeid(jboolean))
//            value = jint2jbool(ISLOT(s));
//        if (typeid(T) == typeid(jchar))
//            value = jint2jchar(ISLOT(s));
//        if (typeid(T) == typeid(jshort))
//            value = jint2jshort(ISLOT(s));
//        if (typeid(T) == typeid(jint))
//            value = ISLOT(s);
//        if (typeid(T) == typeid(jlong))
//            value = LSLOT(s);
//        if (typeid(T) == typeid(jfloat))
//            value = FSLOT(s);
//        if (typeid(T) == typeid(jdouble))
//            value = DSLOT(s);
//        if (typeid(T) == typeid(jref))
//            value = RSLOT(s);
//        else {
//            // todo error
//            jvm_abort("never go here!");
//        }
//    }
}



//static inline slot_t to_fslot(jfloat v)
//{
//    slot_t s;
//    FSLOT(&s) = v;
//    return s;
//}

//static inline slot_t slot(jlong v)
//{
//    slot_t s[2];
//    LSLOT(s) = v;
//    return s;
//}
//
//static inline slot_t slot(jdouble v)
//{
//    slot_t s[2];
//    DSLOT(s) = v;
//    return s;
//}


#endif //KAYO_SLOT_H
