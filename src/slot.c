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

char* slot_to_string(const struct slot *s)
{
#define MAX_LEN 31  // big enough for all cases.
    VM_MALLOCS(char, MAX_LEN + 1, result);
    int n;

    if (s == NULL) {
        n = snprintf(result, MAX_LEN, "slot: NULL");
    } else if (s->t == JINT || s->t == JBYTE || s->t == JCHAR || s->t == JSHORT) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %d).", get_jtype_name(s->t), s->v.i);
    } else if (s->t == JBOOL) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %s).", get_jtype_name(s->t), s->v.i == 0 ? "false" : "true");
    } else if (s->t == JLONG) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %ld).", get_jtype_name(s->t), (long) s->v.l);
    } else if (s->t == JFLOAT) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %f).", get_jtype_name(s->t), s->v.f);
    } else if (s->t == JDOUBLE) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %f).", get_jtype_name(s->t), s->v.d);
    } else if (s->t == JREF) {
        n = snprintf(result, MAX_LEN, "slot: (%s, %p).", get_jtype_name(s->t), s->v.r);
    } else if (s->t == PH || s->t == NAT) {
        n = snprintf(result, MAX_LEN, "slot: %s", get_jtype_name(s->t));
    } else {
        // Never goes here.
        n = snprintf(result, MAX_LEN, "slot: illegal slot. t = %d", s->t);
    }

    if (n < 0) {
        jvm_abort("snprintf 出错\n"); // todo
    }

    assert(0 <= n && n <= MAX_LEN);
    result[n] = 0;
#undef MAX_LEN

    return result;
}
