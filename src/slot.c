/*
 * Author: Jia Yang
 */

#include <mem.h>
#include "slot.h"

#define SLOT_GET(TYPE, type, t0) \
type slot_get##t0(const struct slot *s) \
{ \
    assert(s != NULL); \
    if (s->t == (TYPE)) { \
        return s->v.t0; \
    } \
    jvm_abort("type mismatch. wants %s, gets %s\n", #type, get_jtype_name(s->t)); \
}

SLOT_GET(JINT, jint, i)
SLOT_GET(JFLOAT, jfloat, f)
SLOT_GET(JLONG, jlong, l)
SLOT_GET(JDOUBLE, jdouble, d)
SLOT_GET(JREF, jref, r)

char* slot_to_string(const struct slot *s, char **result)
{
#define MAX_LEN 32  // 32 is big enough for all cases.
    *result = malloc(sizeof(char) * MAX_LEN);   // todo NULL
    int n = -1;

    if (s == NULL) {
        strcpy(*result, "slot: NULL");
    } else if (s->t == JINT || s->t == JBYTE || s->t == JCHAR || s->t == JSHORT) {
        n = sprintf(*result, "slot: %s, %d\0", get_jtype_name(s->t), s->v.i);
    } else if (s->t == JBOOL) {
        n = sprintf(*result, "slot: %s, %s\0", get_jtype_name(s->t), s->v.i == 0 ? "false" : "true");
    } else if (s->t == JLONG) {
        n = sprintf(*result, "slot: %s, %ld\0", get_jtype_name(s->t), (long) s->v.l);
    } else if (s->t == JFLOAT) {
        n = sprintf(*result, "slot: %s, %f\0", get_jtype_name(s->t), s->v.f);
    } else if (s->t == JDOUBLE) {
        n = sprintf(*result, "slot: %s, %f\0", get_jtype_name(s->t), s->v.d);
    } else if (s->t == JREF) {
        n = sprintf(*result, "slot: %s, %p\0", get_jtype_name(s->t), s->v.r);
    } else if (s->t == PH || s->t == NAT) {
        n = sprintf(*result, "slot: %s\0", get_jtype_name(s->t));
    } else {
        // Never goes here.
        n = sprintf(*result, "slot: illegal slot. t = %d\0", s->t);
    }

    if (n >= MAX_LEN) {
        jvm_abort("Never goes here. %d\n", n);
    }

    if (n > 0) {
        *result[n] = 0;
    }
#undef MAX_LEN

    return *result;
}
