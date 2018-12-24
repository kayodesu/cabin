/*
 * Author: Jia Yang
 */

#ifndef JVM_STORES_H
#define JVM_STORES_H

#include "../rtda/thread/frame.h"

jint fetch_index(struct frame *frame);

static inline void __istore(struct frame *frame, int index)
{
    struct slot *s = frame_stack_pop_slot(frame); // os_pops(frame->operand_stack);
    if (s->t == JINT) {
        frame_locals_set(frame, index, s);
        return;
    }

    VM_UNKNOWN_ERROR("slot's type is mismatch, wants \'jint\', gets \'%s\'.\n", slot_to_string(s));
}

static void istore(struct frame *frame)   { __istore(frame, fetch_index(frame)); }
static void istore_0(struct frame *frame) { __istore(frame, 0); }
static void istore_1(struct frame *frame) { __istore(frame, 1); }
static void istore_2(struct frame *frame) { __istore(frame, 2); }
static void istore_3(struct frame *frame) { __istore(frame, 3); }

/////////////////////////////

static inline void __fstore(struct frame *frame, int index)
{
    struct slot *s = frame_stack_pop_slot(frame); // os_pops(frame->operand_stack);
    if (s->t == JFLOAT) {
        frame_locals_set(frame, index, s);
        return;
    }

    VM_UNKNOWN_ERROR("slot's type is mismatch, wants \'jfloat\', gets \'%s\'.\n", slot_to_string(s));
}

static void fstore(struct frame *frame)   { __fstore(frame, fetch_index(frame)); }
static void fstore_0(struct frame *frame) { __fstore(frame, 0); }
static void fstore_1(struct frame *frame) { __fstore(frame, 1); }
static void fstore_2(struct frame *frame) { __fstore(frame, 2); }
static void fstore_3(struct frame *frame) { __fstore(frame, 3); }

/////////////////////////////

static inline void __astore(struct frame *frame, int index)
{
    struct slot *s = frame_stack_pop_slot(frame); // os_pops(frame->operand_stack);
    if (s->t == JREF) {
        frame_locals_set(frame, index, s);
        return;
    }

    VM_UNKNOWN_ERROR("slot's type is mismatch, wants \'reference\', gets \'%s\'.\n", slot_to_string(s));
}

static void astore(struct frame *frame)   { __astore(frame, fetch_index(frame)); }
static void astore_0(struct frame *frame) { __astore(frame, 0); }
static void astore_1(struct frame *frame) { __astore(frame, 1); }
static void astore_2(struct frame *frame) { __astore(frame, 2); }
static void astore_3(struct frame *frame) { __astore(frame, 3); }

/////////////////////////////

static void __lstore(struct frame *frame, int index)
{
    struct slot *s = frame_stack_pop_slot(frame); // os_pops(frame->operand_stack);
    if (s->t != PH) {
        VM_UNKNOWN_ERROR("wants placeholder, gets %s.", slot_to_string(s));
    }

    s = frame_stack_pop_slot(frame);
    if (s->t != JLONG) {
        VM_UNKNOWN_ERROR("slot's type is mismatch, wants \'jlong\', gets \'%s\'.\n", slot_to_string(s));
    }

    frame_locals_set(frame, index, s);
}

static void lstore(struct frame *frame)   { __lstore(frame, fetch_index(frame)); }
static void lstore_0(struct frame *frame) { __lstore(frame, 0); }
static void lstore_1(struct frame *frame) { __lstore(frame, 1); }
static void lstore_2(struct frame *frame) { __lstore(frame, 2); }
static void lstore_3(struct frame *frame) { __lstore(frame, 3); }

/////////////////////////////

static void __dstore(struct frame *frame, int index)
{
    struct slot *s = frame_stack_pop_slot(frame); // os_pops(frame->operand_stack);
    if (s->t != PH) {
        VM_UNKNOWN_ERROR("wants placeholder, gets %s.", slot_to_string(s));
    }

    s = frame_stack_pop_slot(frame);
    if (s->t != JDOUBLE) {
        VM_UNKNOWN_ERROR("slot's type is mismatch, wants \'jdouble\', gets \'%s\'.\n", slot_to_string(s));
    }

    frame_locals_set(frame, index, s);
}

static void dstore(struct frame *frame)   { __dstore(frame, fetch_index(frame)); }
static void dstore_0(struct frame *frame) { __dstore(frame, 0); }
static void dstore_1(struct frame *frame) { __dstore(frame, 1); }
static void dstore_2(struct frame *frame) { __dstore(frame, 2); }
static void dstore_3(struct frame *frame) { __dstore(frame, 3); }

//////////////////////////////////////////////////////////////////

#define __tstore(func_name, frame_stack_pop, check_type, raw_type) \
static void func_name(struct frame *frame) \
{ \
    raw_type value = frame_stack_pop(frame); \
    jint index = frame_stack_popi(frame); \
    struct jobject *ao = frame_stack_popr(frame); \
    if (ao == NULL) { \
        jthread_throw_null_pointer_exception(frame->thread); \
    } \
     \
    if (!check_type(ao->jclass)) { \
        vm_unknown_error("is not array"); \
    } \
    if (!jarrobj_check_bounds(ao, index)) { \
        jthread_throw_array_index_out_of_bounds_exception(frame->thread, index); \
    } \
    *(raw_type *)jarrobj_index(ao, index) = value; \
}

__tstore(iastore, frame_stack_popi, is_int_array, jint)
__tstore(lastore, frame_stack_popl, is_long_array, jlong)
__tstore(fastore, frame_stack_popf, is_float_array, jfloat)
__tstore(dastore, frame_stack_popd, is_double_array, jdouble)
__tstore(aastore, frame_stack_popr, is_ref_array, jref)
__tstore(bastore, frame_stack_popi, is_bool_or_byte_array, jbyte)
__tstore(castore, frame_stack_popi, is_char_array, jchar)
__tstore(sastore, frame_stack_popi, is_short_array, jshort)

#endif //JVM_STORES_H
