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

struct slot* os_top(struct operand_stack *os)
{
    assert(os != NULL);

    if (os->size == 0) {
        printvm("operand stack is empty\n");
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

jint os_popi(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != JINT) {
        // todo error
        jvm_abort("error");
    }
    return s->v.i;
}

jfloat os_popf(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != JFLOAT) {
        // todo error
        jvm_abort("error");
    }
    return s->v.f;
}

jlong os_popl(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != PH) {
        // todo error
        jvm_abort("error");
    }
    s = os_pops(os);
    if (s == NULL || s->t != JLONG) {
        // todo error
        jvm_abort("error");
    }
    return s->v.l;
}

jdouble os_popd(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != PH) {
        // todo error
        jvm_abort("error");
    }
    s = os_pops(os);
    if (s == NULL || s->t != JDOUBLE) {
        // todo error
        jvm_abort("error");
    }
    return s->v.d;
}

jref os_popr(struct operand_stack *os)
{
    assert(os != NULL);

    struct slot *s = os_pops(os);
    if (s == NULL || s->t != REFERENCE) {
        // todo error
        jvm_abort("error");
    }
    return s->v.r;
}

void os_pushi(struct operand_stack *os, jint i)
{
    assert(os != NULL);
    os->slots[os->size++] = islot(i);
}

void os_pushf(struct operand_stack *os, jfloat f)
{
    assert(os != NULL);
    os->slots[os->size++] = fslot(f);
}

// long consumes two slots
void os_pushl(struct operand_stack *os, jlong l)
{
    assert(os != NULL);
    os->slots[os->size++] = lslot(l);
    os->slots[os->size++] = phslot();
}

// double consumes two slots
void os_pushd(struct operand_stack *os, jdouble d)
{
    assert(os != NULL);

    os->slots[os->size++] = dslot(d);
    os->slots[os->size++] = phslot();
}

void os_pushr(struct operand_stack *os, jref r)
{
    assert(os != NULL);
    os->slots[os->size++] = rslot(r);
}

void os_pushs(struct operand_stack *os, const struct slot *s)
{
    assert(os != NULL);
    assert(s != NULL);
    os->slots[os->size++] = *s;
}


