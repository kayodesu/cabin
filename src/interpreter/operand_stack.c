/*
 * Author: Jia Yang
 */

#include "operand_stack.h"
#include "../slot.h"


struct operand_stack* os_create(u2 capacity)
{
    struct operand_stack *os = malloc(sizeof(*os)); // todo NULL
    os->capacity = capacity;
    os->size = 0;
    os->slots = malloc(sizeof(struct slot) * capacity); // todo NULL
    return os;
}

void os_destroy(struct operand_stack *os)
{
    // todo
}

const struct slot* os_top(struct operand_stack *os)
{
    assert(os != NULL);

    if (os->size == 0) {
        jvm_abort("operand stack is empty\n");
        return NULL;
    }
    return os->slots + os->size - 1;
}

struct slot* os_pops(struct operand_stack *os)
{
    assert(os != NULL);

    if (os->size == 0) {
        printvm("operand stack is empty\n");
        return NULL;
    }
    return os->slots + --os->size;
}

#define TYPE_MISMATCH_ERROR(s, wants_type) \
    jvm_abort("type mismatch. wants %s, gets %s", wants_type, (s) == NULL ? "NULL" : get_jtype_name((s)->t))

jint os_popi(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != JINT) {
        TYPE_MISMATCH_ERROR(s, "jint");
    }
    return s->v.i;
}

jfloat os_popf(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != JFLOAT) {
        TYPE_MISMATCH_ERROR(s, "jfloat");
    }
    return s->v.f;
}

jlong os_popl(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != PH) {
        TYPE_MISMATCH_ERROR(s, "placeholder");
    }
    s = os_pops(os);
    if (s == NULL || s->t != JLONG) {
        TYPE_MISMATCH_ERROR(s, "jlong");
    }
    return s->v.l;
}

jdouble os_popd(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != PH) {
        TYPE_MISMATCH_ERROR(s, "placeholder");
    }
    s = os_pops(os);
    if (s == NULL || s->t != JDOUBLE) {
        TYPE_MISMATCH_ERROR(s, "jdouble");
    }
    return s->v.d;
}

jref os_popr(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != REFERENCE) {
        TYPE_MISMATCH_ERROR(s, "reference");
    }

    return s->v.r;
}

#define CHECK_FULL(os) do { if ((os)->size == (os)->capacity) jvm_abort("operand stack is full\n"); } while(false)

void os_pushi(struct operand_stack *os, jint i)
{
    assert(os != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = islot(i);
}

void os_pushf(struct operand_stack *os, jfloat f)
{
    assert(os != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = fslot(f);
}

// long consumes two slots
void os_pushl(struct operand_stack *os, jlong l)
{
    assert(os != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = lslot(l);
    os->slots[os->size++] = phslot();
}

// double consumes two slots
void os_pushd(struct operand_stack *os, jdouble d)
{
    assert(os != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = dslot(d);
    os->slots[os->size++] = phslot();
}

void os_pushr(struct operand_stack *os, jref r)
{
    assert(os != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = rslot(r);
}

void os_pushs(struct operand_stack *os, const struct slot *s)
{
    assert(os != NULL);
    assert(s != NULL);
    CHECK_FULL(os);
    os->slots[os->size++] = *s;
}


