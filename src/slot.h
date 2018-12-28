/*
 * Author: Jia Yang
 */

#ifndef JVM_SLOT_H
#define JVM_SLOT_H

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
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

#define islot(i0) ((struct slot) { .t = JINT,    .v.i = (i0) })
#define fslot(f0) ((struct slot) { .t = JFLOAT,  .v.f = (f0) })
#define lslot(l0) ((struct slot) { .t = JLONG,   .v.l = (l0) })
#define dslot(d0) ((struct slot) { .t = JDOUBLE, .v.d = (d0) })
#define rslot(r0) ((struct slot) { .t = JREF,    .v.r = (r0) })
#define phslot    ((struct slot) { .t = PH })
#define nullslot  ((struct slot) { .t = NAT })

static inline bool slot_is_null(const struct slot *s)
{
    assert(s != NULL);
    return s->t == NAT;
}

#if (JVM_DEBUG)
static inline void slot_ensure_type(const struct slot *s, enum jtype t)
{
    if (s == NULL || s->t != t)
        jvm_abort("type mismatch %d, %d", s->t, t); // todo
}
#else
#define slot_ensure_type(s, t)
#endif

static inline jint slot_geti(const struct slot *s)
{
    slot_ensure_type(s, JINT);
    return s->v.i;
}

static inline jfloat slot_getf(const struct slot *s)
{
    slot_ensure_type(s, JFLOAT);
    return s->v.f;
}

static inline jlong slot_getl(const struct slot *s)
{
    slot_ensure_type(s, JLONG);
    return s->v.l;
}

static inline jdouble slot_getd(const struct slot *s)
{
    slot_ensure_type(s, JDOUBLE);
    return s->v.d;
}

static inline jref slot_getr(const struct slot *s)
{
    slot_ensure_type(s, JREF);
    return s->v.r;
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

static inline bool slots_are_category_two_and_ph(const struct slot *two, const struct slot *ph)
{
    return slot_is_category_two(two) && slot_is_ph(ph);
}

static inline bool slot_is_category_one(const struct slot *s)
{
    assert(s != NULL);
    return !slot_is_ph(s) && !slot_is_category_two(s);
}

static inline bool slots_are_category_one(int num, ...)
{
    va_list valist;
    va_start(valist, num);

    for (int i = 0; i < num; i++) {
       if (!slot_is_category_one(va_arg(valist, struct slot *)))
            return false;
    }

    va_end(valist);
    return true;
}

#if 0
static inline void slot_i2l(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->t = JLONG;
    s->v.l = i2l(s->v.i);
}

static inline void slot_i2f(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->t = JFLOAT;
    s->v.f = i2f(s->v.i);
}

static inline void slot_i2d(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->t = JDOUBLE;
    s->v.d = i2d(s->v.i);
}

static inline void slot_l2i(struct slot *s)
{
    slot_ensure_type(s, JLONG);
    s->t = JINT;
    s->v.i = l2i(s->v.l);
}

static inline void slot_l2f(struct slot *s)
{
    slot_ensure_type(s, JLONG);
    s->t = JFLOAT;
    s->v.f = l2f(s->v.l);
}

static inline void slot_l2d(struct slot *s)
{
    slot_ensure_type(s, JLONG);
    s->t = JDOUBLE;
    s->v.d = l2d(s->v.l);
}

static inline void slot_f2i(struct slot *s)
{
    slot_ensure_type(s, JFLOAT);
    s->t = JINT;
    s->v.i = f2i(s->v.f);
}

static inline void slot_f2l(struct slot *s)
{
    slot_ensure_type(s, JFLOAT);
    s->t = JLONG;
    s->v.l = f2l(s->v.f);
}

static inline void slot_f2d(struct slot *s)
{
    slot_ensure_type(s, JFLOAT);
    s->t = JDOUBLE;
    s->v.d = f2d(s->v.f);
}

static inline void slot_d2i(struct slot *s)
{
    slot_ensure_type(s, JDOUBLE);
    s->t = JINT;
    s->v.i = d2i(s->v.d);
}

static inline void slot_d2l(struct slot *s)
{
    slot_ensure_type(s, JDOUBLE);
    s->t = JLONG;
    s->v.l = d2l(s->v.d);
}

static inline void slot_d2f(struct slot *s)
{
    slot_ensure_type(s, JDOUBLE);
    s->t = JFLOAT;
    s->v.f = d2f(s->v.d);
}

static inline void slot_i2b(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->v.i = i2b(s->v.i);
}

static inline void slot_i2c(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->v.i = i2c(s->v.i);
}

static inline void slot_i2s(struct slot *s)
{
    slot_ensure_type(s, JINT);
    s->v.i = i2s(s->v.i);
}
#endif

/*
 * 由调用者 free result
 */
char* slot_to_string(const struct slot *s);

#endif //JVM_SLOT_H
