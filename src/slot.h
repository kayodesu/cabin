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

static inline char* slot_to_string(const struct slot *s)
{
    if (s == NULL) {
        return "slot: NULL";
    }

    switch (s->t) {
        case JINT:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jint, %d\n", s->v.i);
            break;
        case JBYTE:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jbyte, %d\n", s->v.i);
            break;
        case JBOOL:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jbool, %s\n", s->v.i == 0 ? "false" : "true");
            break;
        case JCHAR:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jchar, %c\n", s->v.i);
            break;
        case JSHORT:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jshort, %d\n", s->v.i);
            break;
        case JLONG:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jlong, %ld\n", (long) s->v.l);
            break;
        case JFLOAT:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jfloat, %f\n", s->v.f);
            break;
        case JDOUBLE:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: jdouble, %f\n", s->v.d);
            break;
        case REFERENCE:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: reference, %p\n", s->v.r);
            break;
        case PH:
        case NAT:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: %s\n", get_jtype_name(s->t));
            break;
        default:
            snprintf(global_buf, GLOBAL_BUF_LEN, "slot: error. t = %d\n", s->t);
            break;
    }

    return global_buf;
}

#endif //JVM_SLOT_H
