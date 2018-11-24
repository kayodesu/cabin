/*
 * Author: Jia Yang
 */

#ifndef JVM_STORES_H
#define JVM_STORES_H

#include "../interpreter/stack_frame.h"

jint fetch_index(struct stack_frame *frame);

static inline void __istore(struct stack_frame *frame, int index)
{
    struct slot *s = os_pops(frame->operand_stack);
    if (s != NULL && s->t == JINT) {
        sf_set_local_var(frame, index, s);
        return;
    }

    // todo error
    jvm_abort("error\n");
}

static void istore(struct stack_frame *frame)   { __istore(frame, fetch_index(frame)); }
static void istore_0(struct stack_frame *frame) { __istore(frame, 0); }
static void istore_1(struct stack_frame *frame) { __istore(frame, 1); }
static void istore_2(struct stack_frame *frame) { __istore(frame, 2); }
static void istore_3(struct stack_frame *frame) { __istore(frame, 3); }

/////////////////////////////

static inline void __fstore(struct stack_frame *frame, int index)
{
    struct slot *s = os_pops(frame->operand_stack);
    if (s != NULL && s->t == JFLOAT) {
        sf_set_local_var(frame, index, s);
        return;
    }

    // todo error
    jvm_abort("error\n");
}

static void fstore(struct stack_frame *frame)   { __fstore(frame, fetch_index(frame)); }
static void fstore_0(struct stack_frame *frame) { __fstore(frame, 0); }
static void fstore_1(struct stack_frame *frame) { __fstore(frame, 1); }
static void fstore_2(struct stack_frame *frame) { __fstore(frame, 2); }
static void fstore_3(struct stack_frame *frame) { __fstore(frame, 3); }

/////////////////////////////

static inline void __astore(struct stack_frame *frame, int index)
{
    struct slot *s = os_pops(frame->operand_stack);
    if (s != NULL && s->t == JREF) {
        sf_set_local_var(frame, index, s);
        return;
    }

    // todo error
    if (s == NULL) {
        jvm_abort("operand stack is empty.\n");
    } else {
        jvm_abort("slot's type is mismatch, wants \'reference\', gets \'%s\'.\n", get_jtype_name(s->t));
    }
}

static void astore(struct stack_frame *frame)   { __astore(frame, fetch_index(frame)); }
static void astore_0(struct stack_frame *frame) { __astore(frame, 0); }
static void astore_1(struct stack_frame *frame) { __astore(frame, 1); }
static void astore_2(struct stack_frame *frame) { __astore(frame, 2); }
static void astore_3(struct stack_frame *frame) { __astore(frame, 3); }

/////////////////////////////

static void __lstore(struct stack_frame *frame, int index)
{
    struct slot *s = os_pops(frame->operand_stack);
    if (s == NULL || s->t != PH) {
        // todo error
        SLOT_TO_STRING_WRAP(s, jvm_abort("wants placeholder, gets %s.", slot_str));
    }

    s = os_pops(frame->operand_stack);
    if (s == NULL || s->t != JLONG) {
        // todo error
        SLOT_TO_STRING_WRAP(s, jvm_abort("wants jlong, gets %s.", slot_str));
    }

    sf_set_local_var(frame, index, s);
}

static void lstore(struct stack_frame *frame)   { __lstore(frame, fetch_index(frame)); }
static void lstore_0(struct stack_frame *frame) { __lstore(frame, 0); }
static void lstore_1(struct stack_frame *frame) { __lstore(frame, 1); }
static void lstore_2(struct stack_frame *frame) { __lstore(frame, 2); }
static void lstore_3(struct stack_frame *frame) { __lstore(frame, 3); }

/////////////////////////////

static void __dstore(struct stack_frame *frame, int index)
{
    struct slot *s = os_pops(frame->operand_stack);
    if (s == NULL || s->t != PH) {
        // todo error
        jvm_abort("error");
    }

    s = os_pops(frame->operand_stack);
    if (s == NULL || s->t != JDOUBLE) {
        // todo error
        jvm_abort("error");
    }

    sf_set_local_var(frame, index, s);
}

static void dstore(struct stack_frame *frame)   { __dstore(frame, fetch_index(frame)); }
static void dstore_0(struct stack_frame *frame) { __dstore(frame, 0); }
static void dstore_1(struct stack_frame *frame) { __dstore(frame, 1); }
static void dstore_2(struct stack_frame *frame) { __dstore(frame, 2); }
static void dstore_3(struct stack_frame *frame) { __dstore(frame, 3); }

//////////////////////////////////////////////////////////////////

#define __tstore(func_name, os_pop, check_type, raw_type) \
static void func_name(struct stack_frame *frame) \
{ \
    raw_type value = os_pop(frame->operand_stack); \
    jint index = os_popi(frame->operand_stack); \
    struct jobject *ao = os_popr(frame->operand_stack); \
    if (ao == NULL) { \
        jvm_abort("error NULL Point Exception\n"); /* todo */ \
    } \
    if (ao->t != ARRAY_OBJECT) { \
        jvm_abort("error\n"); /* todo */ \
    } \
     \
    if (!check_type(ao->jclass)) { \
        jvm_abort("error\n"); \
    } \
    if (!jarrobj_check_bounds(ao, index)) { \
        jvm_abort("ArrayIndexOutOfBoundsException\n"); \
        /* todo throw new ArrayIndexOutOfBoundsException(String.valueOf(index)); */ \
    } \
    *(raw_type *)jarrobj_index(ao, index) = value; \
}

__tstore(iastore, os_popi, is_int_array, jint)
__tstore(lastore, os_popl, is_long_array, jlong)
__tstore(fastore, os_popf, is_float_array, jfloat)
__tstore(dastore, os_popd, is_double_array, jdouble)
__tstore(aastore, os_popr, is_ref_array, jref)
__tstore(bastore, os_popi, is_bool_or_byte_array, jbyte)
__tstore(castore, os_popi, is_char_array, jchar)
__tstore(sastore, os_popi, is_short_array, jshort)

#endif //JVM_STORES_H
