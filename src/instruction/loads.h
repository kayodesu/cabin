/*
 * Author: Jia Yang
 */

#ifndef JVM_LOADS_H
#define JVM_LOADS_H

#include "../rtda/thread/frame.h"
#include "../rtda/heap/jobject.h"

jint fetch_index(struct frame *frame);

/////////////////////////////

static inline void __iload(struct frame *frame, int index)
{
    frame_stack_pushi(frame, frame_locals_geti(frame, index));
}

static void iload(struct frame *frame)   { __iload(frame, fetch_index(frame)); }
static void iload_0(struct frame *frame) { __iload(frame, 0); }
static void iload_1(struct frame *frame) { __iload(frame, 1); }
static void iload_2(struct frame *frame) { __iload(frame, 2); }
static void iload_3(struct frame *frame) { __iload(frame, 3); }

/////////////////////////////

static inline void __lload(struct frame *frame, int index)
{
    frame_stack_pushl(frame, frame_locals_getl(frame, index));
}

static void lload(struct frame *frame)   { __lload(frame, fetch_index(frame)); }
static void lload_0(struct frame *frame) { __lload(frame, 0); }
static void lload_1(struct frame *frame) { __lload(frame, 1); }
static void lload_2(struct frame *frame) { __lload(frame, 2); }
static void lload_3(struct frame *frame) { __lload(frame, 3); }

/////////////////////////////

static inline void __fload(struct frame *frame, int index)
{
    frame_stack_pushf(frame, frame_locals_getf(frame, index));
}

static void fload(struct frame *frame)   { __fload(frame, fetch_index(frame)); }
static void fload_0(struct frame *frame) { __fload(frame, 0); }
static void fload_1(struct frame *frame) { __fload(frame, 1); }
static void fload_2(struct frame *frame) { __fload(frame, 2); }
static void fload_3(struct frame *frame) { __fload(frame, 3); }

/////////////////////////////

static inline void __dload(struct frame *frame, int index)
{
    frame_stack_pushd(frame, frame_locals_getd(frame, index));
}

static void dload(struct frame *frame)   { __dload(frame, fetch_index(frame)); }
static void dload_0(struct frame *frame) { __dload(frame, 0); }
static void dload_1(struct frame *frame) { __dload(frame, 1); }
static void dload_2(struct frame *frame) { __dload(frame, 2); }
static void dload_3(struct frame *frame) { __dload(frame, 3); }


/////////////////////////////

static inline void __aload(struct frame *frame, int index)
{
    frame_stack_pushr(frame, frame_locals_getr(frame, index));
}

static void aload(struct frame *frame)   { __aload(frame, fetch_index(frame)); }
static void aload_0(struct frame *frame) { __aload(frame, 0); }
static void aload_1(struct frame *frame) { __aload(frame, 1); }
static void aload_2(struct frame *frame) { __aload(frame, 2); }
static void aload_3(struct frame *frame) { __aload(frame, 3); }

#define __taload(func_name, check_type, raw_type) \
static void func_name(struct frame *frame) \
{ \
    jint index = frame_stack_popi(frame); \
    struct jobject *ao = frame_stack_popr(frame); \
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
    frame_stack_push(frame, *(raw_type *)jarrobj_index(ao, index)); \
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
