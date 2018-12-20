/*
 * Author: Jia Yang
 */

#ifndef JVM_MATHS_H
#define JVM_MATHS_H

#include <math.h>
#include "../interpreter/stack_frame.h"
#include "../slot.h"

#define ICAL(func_name, oper) \
    static void func_name(struct stack_frame *frame) \
    { \
        jint v2 = os_popi(frame->operand_stack); \
        jint v1 = os_popi(frame->operand_stack); \
        os_pushi(frame->operand_stack, v1 oper v2); /* todo 相加溢出的问题 */ \
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
    static void func_name(struct stack_frame *frame) \
    { \
        jlong v2 = os_popl(frame->operand_stack); \
        jlong v1 = os_popl(frame->operand_stack); \
        os_pushl(frame->operand_stack, v1 oper v2); /* todo 相加溢出的问题 */ \
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
    static void func_name(struct stack_frame *frame) \
    { \
        jfloat v2 = os_popf(frame->operand_stack); \
        jfloat v1 = os_popf(frame->operand_stack); \
        os_pushf(frame->operand_stack, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

FCAL(__fadd, +)
FCAL(__fsub, -)
FCAL(__fmul, *)
FCAL(__fdiv, /)

static void __frem(struct stack_frame *frame)
{
    jfloat v2 = os_popf(frame->operand_stack);
    jfloat v1 = os_popf(frame->operand_stack);
    jvm_abort("not implement\n");
//    os_pushf(frame->operand_stack, dremf(v1, v2)); /* todo 相加溢出的问题 */
}

////////////////////////////

#define DCAL(func_name, oper) \
    static void func_name(struct stack_frame *frame) \
    { \
        jdouble v2 = os_popd(frame->operand_stack); \
        jdouble v1 = os_popd(frame->operand_stack); \
        os_pushd(frame->operand_stack, v1 oper v2); /* todo 相加溢出的问题 */ \
    }

DCAL(__dadd, +)
DCAL(__dsub, -)
DCAL(__dmul, *)
DCAL(__ddiv, /)

static void __drem(struct stack_frame *frame)
{
    jdouble v2 = os_popd(frame->operand_stack);
    jdouble v1 = os_popd(frame->operand_stack);
    jvm_abort("未实现\n");
//    os_pushd(frame->operand_stack, drem(v1, v2)); /* todo 相加溢出的问题 */
}

#define NEG(T) \
    static void T##neg(struct stack_frame *frame) \
    { \
        os_push##T(frame->operand_stack, -os_pop##T(frame->operand_stack)); \
    }

NEG(i)
NEG(l)
NEG(f)
NEG(d)

static void ishl(struct stack_frame *frame)
{
    // 与0x1f是因为低5位表示位移距离，位移距离实际上被限制在0到31之间。
    jint shift = os_popi(frame->operand_stack) & 0x1f;
    jint value = os_popi(frame->operand_stack);
    os_pushi(frame->operand_stack, value << shift);
}

static void lshl(struct stack_frame *frame)
{
    // 与0x3f是因为低6位表示位移距离，位移距离实际上被限制在0到63之间。
    jint shift = os_popi(frame->operand_stack) & 0x3f;
    jlong value = os_popl(frame->operand_stack);
    os_pushl(frame->operand_stack, value << shift);
}

// 逻辑右移 shift logical right
static void ishr(struct stack_frame *frame)
{
    jint shift = os_popi(frame->operand_stack) & 0x1f;
    jint value = os_popi(frame->operand_stack);
    os_pushi(frame->operand_stack, (~(((jint)1) >> shift)) & (value >> shift));
}

static void lshr(struct stack_frame *frame)
{
    jint shift = os_popi(frame->operand_stack) & 0x3f;
    jlong value = os_popl(frame->operand_stack);
    os_pushl(frame->operand_stack, (~(((jlong)1) >> shift)) & (value >> shift));
}

// 算术右移 shift arithmetic right
static void iushr(struct stack_frame *frame)
{
    jint shift = os_popi(frame->operand_stack) & 0x1f;
    jint value = os_popi(frame->operand_stack);
    os_pushi(frame->operand_stack, value >> shift);
}

static void lushr(struct stack_frame *frame)
{
    jint shift = os_popi(frame->operand_stack) & 0x3f;
    jlong value = os_popl(frame->operand_stack);
    os_pushl(frame->operand_stack, value >> shift);
}

extern bool wide_extending;

static void iinc(struct stack_frame *frame)
{
    jint index, value;

    if (wide_extending) {
        index = bcr_readu2(frame->reader);
        value = bcr_reads2(frame->reader);
        wide_extending = false;
    } else {
        index = bcr_readu1(frame->reader);
        value = bcr_reads1(frame->reader);
    }

    struct slot *s = frame->local_vars + index;
    if (s->t != JINT) {
        VM_UNKNOWN_ERROR("type mismatch. wants %d, but gets %d", JINT, s->t);
    }

    s->v.i += value;
//    printvm("iinc: index is %d, value is %d, result is %d\n", index, value, s->v.i);
}

#endif //JVM_MATHS_H
