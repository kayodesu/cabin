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
 * 在 slot 中，jbool, jbyte, jchar, jshort 统统用 jint 表示。
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

#define islot(i0) ((struct slot) { .t = JINT, .v.i = (i0) })
#define fslot(f0) ((struct slot) { .t = JFLOAT, .v.f = (f0) })
#define lslot(l0) ((struct slot) { .t = JLONG, .v.l = (l0) })
#define dslot(d0) ((struct slot) { .t = JDOUBLE, .v.d = (d0) })
#define rslot(r0) ((struct slot) { .t = REFERENCE, .v.r = (r0) })
#define phslot() ((struct slot)  { .t = PH })
#define natslot() ((struct slot) { .t = NAT })

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
    jvm_abort("type mismatch. wants jint, gets %s\n", get_jtype_name(s->t));
}

static inline jfloat slot_getf(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JFLOAT) {
        return s->v.f;
    }
    jvm_abort("type mismatch. wants jfloat, gets %s\n", get_jtype_name(s->t));
}

static inline jlong slot_getl(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JLONG) {
        return s->v.l;
    }
    jvm_abort("type mismatch. wants jlong, gets %s\n", get_jtype_name(s->t));
}

static inline jdouble slot_getd(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == JDOUBLE) {
        return s->v.d;
    }
    jvm_abort("type mismatch. wants jdouble, gets %s\n", get_jtype_name(s->t));
}

static inline jref slot_getr(const struct slot *s)
{
    assert(s != NULL);
    if (s->t == REFERENCE) {
        return s->v.r;
    }
    jvm_abort("type mismatch. wants reference, gets %s\n", get_jtype_name(s->t));
}

static inline bool slot_is_ph(const struct slot *s)
{
    assert(s != NULL);
    return s->t == PH;
}

static inline bool slot_is_category_two(const struct slot *s)
{
    assert(s != NULL);
    return s->t == JLONG || s->t == JDOUBLE;
}

static inline bool slot_is_category_one(const struct slot *s)
{
    assert(s != NULL);
    return !slot_is_ph(s) && !slot_is_category_two(s);
}

static inline void slot_print(const struct slot *s)
{
    if (s == NULL) {
        printvm("slot: NULL");
        return;
    }

    switch (s->t) {
        case JINT:
            printvm("slot: JINT, %d\n", s->v.i);
            break;
        case JBYTE:
            printvm("slot: JBYTE, %d\n", s->v.i);
            break;
        case JBOOL:
            printvm("slot: JBOOL, %s\n", s->v.i == 0 ? "false" : "true");
            break;
        case JCHAR:
            printvm("slot: JCHAR, %c\n", s->v.i);
            break;
        case JSHORT:
            printvm("slot: JSHORT, %d\n", s->v.i);
            break;
        case JLONG:
            printvm("slot: JLONG, %ld\n", s->v.l);
            break;
        case JFLOAT:
            printvm("slot: JFLOAT, %f\n", s->v.f);
            break;
        case JDOUBLE:
            printvm("slot: JDOUBLE, %f\n", s->v.d);
            break;
        case REFERENCE:
            printvm("slot: REFERENCE, %p\n", s->v.r);
            break;
        case PH:
        case NAT:
            printvm("slot: NAT\n", get_jtype_name(s->t));
            break;
        default:
            printvm("slot: error. t = %d\n", s->t);
            break;
    }
}

#endif //JVM_SLOT_H
