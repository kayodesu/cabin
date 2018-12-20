/*
 * Author: Jia Yang
 */

#ifndef JVM_LOADS_H
#define JVM_LOADS_H

#include "../interpreter/stack_frame.h"
#include "../rtda/heap/jobject.h"

jint fetch_index(struct stack_frame *frame);

/////////////////////////////

static inline void __iload(struct stack_frame *frame, int index)
{
    os_pushi(frame->operand_stack, slot_geti(frame->local_vars + index));
}

static void iload(struct stack_frame *frame)   { __iload(frame, fetch_index(frame)); }
static void iload_0(struct stack_frame *frame) { __iload(frame, 0); }
static void iload_1(struct stack_frame *frame) { __iload(frame, 1); }
static void iload_2(struct stack_frame *frame) { __iload(frame, 2); }
static void iload_3(struct stack_frame *frame) { __iload(frame, 3); }

/////////////////////////////

static inline void __lload(struct stack_frame *frame, int index)
{
    os_pushl(frame->operand_stack, slot_getl(frame->local_vars + index));
}

static void lload(struct stack_frame *frame)   { __lload(frame, fetch_index(frame)); }
static void lload_0(struct stack_frame *frame) { __lload(frame, 0); }
static void lload_1(struct stack_frame *frame) { __lload(frame, 1); }
static void lload_2(struct stack_frame *frame) { __lload(frame, 2); }
static void lload_3(struct stack_frame *frame) { __lload(frame, 3); }

/////////////////////////////

static inline void __fload(struct stack_frame *frame, int index)
{
    os_pushf(frame->operand_stack, slot_getf(frame->local_vars + index));
}

static void fload(struct stack_frame *frame)   { __fload(frame, fetch_index(frame)); }
static void fload_0(struct stack_frame *frame) { __fload(frame, 0); }
static void fload_1(struct stack_frame *frame) { __fload(frame, 1); }
static void fload_2(struct stack_frame *frame) { __fload(frame, 2); }
static void fload_3(struct stack_frame *frame) { __fload(frame, 3); }

/////////////////////////////

static inline void __dload(struct stack_frame *frame, int index)
{
    os_pushd(frame->operand_stack, slot_getd(frame->local_vars + index));
}

static void dload(struct stack_frame *frame)   { __dload(frame, fetch_index(frame)); }
static void dload_0(struct stack_frame *frame) { __dload(frame, 0); }
static void dload_1(struct stack_frame *frame) { __dload(frame, 1); }
static void dload_2(struct stack_frame *frame) { __dload(frame, 2); }
static void dload_3(struct stack_frame *frame) { __dload(frame, 3); }


/////////////////////////////

static inline void __aload(struct stack_frame *frame, int index)
{
    os_pushr(frame->operand_stack, slot_getr(frame->local_vars + index));
}

static void aload(struct stack_frame *frame)   { __aload(frame, fetch_index(frame)); }
static void aload_0(struct stack_frame *frame) { __aload(frame, 0); }
static void aload_1(struct stack_frame *frame) { __aload(frame, 1); }
static void aload_2(struct stack_frame *frame) { __aload(frame, 2); }
static void aload_3(struct stack_frame *frame) { __aload(frame, 3); }

#define __taload(func_name, check_type, raw_type) \
static void func_name(struct stack_frame *frame) \
{ \
    jint index = os_popi(frame->operand_stack); \
    struct jobject *ao = os_popr(frame->operand_stack); \
    if (ao == NULL) { \
        jthread_throw_null_pointer_exception(frame->thread); \
    } \
    \
    if (!check_type(ao->jclass)) { \
        vm_unknown_error("type mismatch"); \
    } \
    \
    if (!jarrobj_check_bounds(ao, index)) { \
        jthread_throw_array_index_out_of_bounds_exception(frame->thread, index); \
    } \
    os_push(frame->operand_stack, *(raw_type *)jarrobj_index(ao, index)); \
}

__taload(iaload, is_int_array, jint)
__taload(laload, is_long_array, jlong)
__taload(faload, is_float_array, jfloat)
__taload(daload, is_double_array, jdouble)
__taload(aaload, is_ref_array, jref)
__taload(baload, is_bool_or_byte_array, jbyte)
__taload(caload, is_char_array, jchar)
__taload(saload, is_short_array, jshort)

#endif //JVM_LOADS_H
