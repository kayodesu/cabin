/*
 * Author: Jia Yang
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include <assert.h>
#include <stdio.h>
#include "jtypes.h"
#include "jvm.h"

/*
 * 在 Slot 中，jbool, jbyte, jchar, jshort 统统用 jint 表示。
 *
 * todo   增加注释： slot 应用在哪里
 */
struct slot {
    enum jtype t;

    union {
        jint i;
        jfloat f;
        jlong l;
        jdouble d;
        jref r;
    } v;
};

#define islot(i0) ((struct slot) { JINT, .v.i = (i0) })
#define fslot(f0) ((struct slot) { JFLOAT, .v.f = (f0) })
#define lslot(l0) ((struct slot) { JLONG, .v.l = (l0) })
#define dslot(d0) ((struct slot) { JDOUBLE, .v.d = (d0) })
#define rslot(r0) ((struct slot) { REFERENCE, .v.r = (r0) })
#define phslot() ((struct slot) { PH })

// 设置 slot 的值
// s: 要设置值的 slot 的地址
// v: 值
//#define slot_set(s, v0) \
//do { \
//    assert((s) != NULL); \
//    _Generic((v0), \
//        jint: *(s) = islot(v0), \
//        jfloat: *(s) = fslot(v0), \
//        jlong: *(s) = lslot(v0), \
//        jdouble: *(s) = dslot(v0), \
//        jref: *(s) = rslot(v0) \
//    );\
//} while (false)

static inline jint slot_geti(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JINT) {
        return s->v.i;
    }
    // todo error
    jvm_abort("fff");
}

static inline jfloat slot_getf(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JFLOAT) {
        return s->v.f;
    }
    // todo error
    jvm_abort("fff");
}

static inline jlong slot_getl(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JLONG) {
        return s->v.l;
    }
    // todo error
    jvm_abort("fff");
}

static inline jdouble slot_getd(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JDOUBLE) {
        return s->v.d;
    }
    // todo error
    jvm_abort("fff");
}

static inline jref slot_getr(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == REFERENCE) {
        return s->v.r;
    }
    // todo error
    jvm_abort("fff");
}

static bool slot_is_ph(const struct slot *s)
{
    assert(s != NULL);
    return s->t == PH;
}

static bool slot_is_category_two(const struct slot *s)
{
    assert(s != NULL);
    return s->t == JLONG || s->t == JDOUBLE;
}

static bool slot_is_category_one(const struct slot *s)
{
    assert(s != NULL);
    return !slot_is_ph(s) && !slot_is_category_two(s);
}

//static inline void slot_seti(struct slot *s, jint i)
//{
//    assert(s);
//    s->t = JINT;
//    s->v.i = i;
//}
//
//static inline void slot_setf(struct slot *s, jfloat f)
//{
//    assert(s);
//    s->t = JFLOAT;
//    s->v.f = f;
//}
//
//static inline void slot_setl(struct slot *s, jlong l)
//{
//    assert(s);
//    s->t = JLONG;
//    s->v.l = l;
//}
//
//static inline void slot_setd(struct slot *s, jdouble d)
//{
//    assert(s);
//    s->t = JDOUBLE;
//    s->v.d = d;
//}
//
//static inline void slot_setr(struct slot *s, jref r)
//{
//    assert(s);
//    s->t = REFERENCE;
//    s->v.r = r;
//}

#endif //JVM_SLOT_H
