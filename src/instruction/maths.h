/*
 * Author: Jia Yang
 */

#ifndef JVM_MATHS_H
#define JVM_MATHS_H

#include <math.h>
#include "../rtda/thread/frame.h"
#include "../slot.h"

#define ICAL(func_name, oper) \
    static void func_name(struct frame *frame) \
    { \
        jint v2 = frame_stack_popi(frame); \
        jint v1 = frame_stack_popi(frame); \
        frame_stack_pushi(frame, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

ICAL(__iadd, +)
ICAL(__isub, -)
ICAL(__imul, *)
ICAL(__idiv, /)
ICAL(__irem, %)
ICAL(__iand, &)
ICAL(__ior,  |)
ICAL(__ixor, ^)

////////////////////////////

#define LCAL(func_name, oper) \
    static void func_name(struct frame *frame) \
    { \
        jlong v2 = frame_stack_popl(frame); \
        jlong v1 = frame_stack_popl(frame); \
        frame_stack_pushl(frame, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

LCAL(__ladd, +)
LCAL(__lsub, -)
LCAL(__lmul, *)
LCAL(__ldiv, /)
LCAL(__lrem, %)
LCAL(__land, &)
LCAL(__lor,  |)
LCAL(__lxor, ^)

////////////////////////////

#define FCAL(func_name, oper) \
    static void func_name(struct frame *frame) \
    { \
        jfloat v2 = frame_stack_popf(frame); \
        jfloat v1 = frame_stack_popf(frame); \
        frame_stack_pushf(frame, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

FCAL(__fadd, +)
FCAL(__fsub, -)
FCAL(__fmul, *)
FCAL(__fdiv, /)

static void __frem(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    jvm_abort("not implement\n");
//    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
}

////////////////////////////

#define DCAL(func_name, oper) \
    static void func_name(struct frame *frame) \
    { \
        jdouble v2 = frame_stack_popd(frame); \
        jdouble v1 = frame_stack_popd(frame); \
        frame_stack_pushd(frame, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

DCAL(__dadd, +)
DCAL(__dsub, -)
DCAL(__dmul, *)
DCAL(__ddiv, /)

static void __drem(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    jvm_abort("未实现\n");
//    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
}

#define NEG(T) \
static void T##neg(struct frame *frame) \
{ \
    frame_stack_push##T(frame, -frame_stack_pop##T(frame)); \
}

NEG(i)
NEG(l)
NEG(f)
NEG(d)

static void ishl(struct frame *frame)
{
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    jint shift =frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, value << shift);
}

static void lshl(struct frame *frame)
{
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, value << shift);
}

// 逻辑右移 shift logical right
static void ishr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, (~(((jint)1) >> shift)) & (value >> shift));
}

static void lshr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, (~(((jlong)1) >> shift)) & (value >> shift));
}

// 算术右移 shift arithmetic right
static void iushr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x1f;
    jint value = frame_stack_popi(frame);
    frame_stack_pushi(frame, value >> shift);
}

static void lushr(struct frame *frame)
{
    jint shift = frame_stack_popi(frame) & 0x3f;
    jlong value = frame_stack_popl(frame);
    frame_stack_pushl(frame, value >> shift);
}

extern bool wide_extending;

static void iinc(struct frame *frame)
{
    jint index, value;

    if (wide_extending) {
        index = bcr_readu2(&frame->reader);
        value = bcr_reads2(&frame->reader);
        wide_extending = false;
    } else {
        index = bcr_readu1(&frame->reader);
        value = bcr_reads1(&frame->reader);
    }

    struct slot *s = frame_locals_get(frame, index);
    if (s->t != JINT) {
        VM_UNKNOWN_ERROR("type mismatch. wants %d, but gets %d", JINT, s->t);
    }

    s->v.i += value;
}

#endif //JVM_MATHS_H
