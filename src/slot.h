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

#define islot(i0) ((struct slot) { .t = JINT,    .v.i = (i0) })
#define fslot(f0) ((struct slot) { .t = JFLOAT,  .v.f = (f0) })
#define lslot(l0) ((struct slot) { .t = JLONG,   .v.l = (l0) })
#define dslot(d0) ((struct slot) { .t = JDOUBLE, .v.d = (d0) })
#define rslot(r0) ((struct slot) { .t = JREF,    .v.r = (r0) })
#define phslot    ((struct slot) { .t = PH })

jint    slot_geti(const struct slot *s);
jfloat  slot_getf(const struct slot *s);
jlong   slot_getl(const struct slot *s);
jdouble slot_getd(const struct slot *s);
jref    slot_getr(const struct slot *s);

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


/*
 * 由调用者 free result
 */
char* slot_to_string(const struct slot *s);

// 对 slot_to_string 函数提供一层包裹
#define SLOT_TO_STRING_WRAP(slot_point, ues_slot_str) \
    do { \
        char *slot_str = slot_to_string(slot_point); \
        ues_slot_str; \
        free(slot_str); \
    } while (false)

#endif //JVM_SLOT_H
