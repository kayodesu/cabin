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
static_assert(2*sizeof(slot_t) >= sizeof(jlong));
static_assert(2*sizeof(slot_t) >= sizeof(jdouble));

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

class SlotsMgr {
    slot_t *slots = nullptr;
    int last = 0;

public:
    SlotsMgr() = default;

    explicit SlotsMgr(slot_t *_slots): slots(_slots)
    {
        assert(_slots != nullptr);
    }

    void init(slot_t *_slots)
    {
        assert(_slots != nullptr);
        slots = _slots;
    }

    /* setter */

    void setInt(int index, jint v)       { ISLOT(slots + index) = v; }
    void setByte(int index, jbyte v)     { setInt (index, v); }
    void setBool(int index, jbool v)     { setInt (index, v); }
    void setChar(int index, jchar v)     { setInt (index, v); }
    void setShort(int index, jshort v)   { setInt (index, v); }
    void setFloat(int index, jfloat v)   { FSLOT(slots + index) = v; }
    void setLong(int index, jlong v)     { LSLOT(slots + index) = v; }
    void setDouble(int index, jdouble v) { DSLOT(slots + index) = v; }
    void setRef(int index, jref v)       { RSLOT(slots + index) = v; }

    /* getter */

    jint getInt(int index)       { return ISLOT(slots + index); }
    jbyte getByte(int index)     { return jint2jbyte(getInt(index)); }
    jbool getBool(int index)     { return jint2jbool(getInt(index)); }
    jchar getChar(int index)     { return jint2jchar(getInt(index)); }
    jshort getShort(int index)   { return jint2jshort(getInt(index)); }
    jfloat getFloat(int index)   { return FSLOT(slots + index); }
    jlong getLong(int index)     { return LSLOT(slots + index); }
    jdouble getDouble(int index) { return DSLOT(slots + index); }
    template <typename T = Object>
    T *getRef(int index)         { return (T *) RSLOT(slots + index); }

    /* pusher */

//    void push(slot_t v)   { *ostack++ = v; }
//    void pushByte(jbyte v)    { pushi(v);  }
//    void pushChar(jchar v)    { pushi(v);   }
//    void pushShort(jshotr v)    { pushi(v);   }
//    void pushInt(jint v)    { ISLOT(ostack) = v; ostack++;   }
//    void pushFloat(jfloat v)  { FSLOT(ostack) = v; ostack++; }
//    void pushLong(jlong v)   { LSLOT(ostack) = v; ostack += 2; }
//    void pushDouble(jdouble v) { DSLOT(ostack) = v; ostack += 2; }
//    void pushRef(jref v)    { RSLOT(ostack) = v; ostack++; }

    //     jint    popi() { ostack--;    return ISLOT(ostack); }
    // jfloat  popf() { ostack--;    return FSLOT(ostack); }
    // jlong   popl() { ostack -= 2; return LSLOT(ostack); }
    // jdouble popd() { ostack -= 2; return DSLOT(ostack); }
    // jref    popr() { ostack--;    return RSLOT(ostack); }

};

#endif //KAYO_SLOT_H
