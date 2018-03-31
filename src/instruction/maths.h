/*
 * 数学指令
 *
 * Author: Jia Yang
 */

#ifndef JVM_MATHS_H
#define JVM_MATHS_H

#include <math.h>

#include "../interpreter/StackFrame.h"

#define MATH_INS(fun_name, pop_type, push_type, operation)\
static void fun_name(StackFrame *frame) {\
    auto t2 = frame->operandStack.pop##pop_type();\
    auto t1 = frame->operandStack.pop##pop_type();\
    frame->operandStack.push((push_type)(operation));\
}

template <typename T, T (OperandStack::*popValue)(), T (*cal)(T, T)>
static void mathcal(StackFrame *frame) {
    T v2 = (frame->operandStack.*popValue)();
    T v1 = (frame->operandStack.*popValue)();
    frame->operandStack.push(cal(v1, v2));
}

/*
 * NAN 与正常的的浮点数无法比较，即即不大于也不小于也不等于。
 * 两个NAN之间也无法比较，即即不大于也不小于也不等于。
 */
template <typename T, T (OperandStack::*popValue)(), jint defaultValue = -1>
static void __cmp(StackFrame *frame) {
    T v2 = (frame->operandStack.*popValue)();
    T v1 = (frame->operandStack.*popValue)();
    frame->operandStack.push((jint)(v1 > v2 ? 1 : (v1 == v2 ? 0 : (v1 < v2 ? -1 : defaultValue))));
}

//template <typename T, T (OperandStack::*popValue)(), T (*cal)(T, T)>
//static void mathcal(StackFrame *frame) {
//    T v2 = (frame->operandStack.*popValue)();
//    T v1 = (frame->operandStack.*popValue)();
//    frame->operandStack.push(cal(v1, v2));
//}

MATH_INS(__irem, Int, jint, t1 % t2)
MATH_INS(__frem, Float, jfloat, fmodf(t1, t2))
MATH_INS(__lrem, Long, jlong, t1 % t2)
MATH_INS(__drem_, Double, jdouble, fmod(t1, t2))  // todo  fmod or drem  ???????!!!!!!!!

// 左移
//template <typename T, T (OperandStack::*pop)()>
//void __shl(StackFrame *frame) {
//    T t2 = (frame->operandStack.*pop)();
//    T t1 = (frame->operandStack.*pop)();
//
//    if (sizeof(t1) == 4) {
//        t2 = t2 & 0x1f; // t2低5位表示位移距离，位移的距离实际上被限制在0到31之间。
//    } else if (sizeof(t1) == 8) {
//        t2 = t2 & 0x3f; // t2低6位表示位移距离，位移的距离实际上被限制在0到63之间。
//    } else {
//        jvmAbort("error, %d\n", sizeof(t2));
//    }
//
//    frame->operandStack.push((T)(t1 << t2));
//}

// 逻辑右移 shift logical right
template <typename T, T (OperandStack::*pop)(), bool logical>
void __shr(StackFrame *frame) {
    jint t2 = frame->operandStack.popInt();
    T t1 = (frame->operandStack.*pop)();

    if (sizeof(t1) == 4) {
        t2 = t2 & 0x1f; // t2低5位表示位移距离，位移的距离实际上被限制在0到31之间。
    } else if (sizeof(t1) == 8) {
        t2 = t2 & 0x3f; // t2低6位表示位移距离，位移的距离实际上被限制在0到63之间。
    } else {
        jvmAbort("error, %d\n", sizeof(t2));
    }

    if (logical) { // 逻辑右移 shift logical right
        frame->operandStack.push((~(((T)1) >> t2)) & (t1 >> t2));
    } else { // 算术右移 shift arithmetic right
        frame->operandStack.push(t1 >> t2);
    }
}

MATH_INS(__lcmp, Long, jint, t1 > t2 ? 1 : (t1 == t2 ? 0 : -1));

template <Jtype jtype>
static void __neg(StackFrame *frame) {
    Slot &s = frame->operandStack.peek();
    if (s.type != jtype) {
        /* todo error*/
        jvmAbort("error.\n");
        return;
    }
    s.negValue();
}

static void __iinc(StackFrame *frame) {
    int index = frame->reader->readu1();
    int value = frame->reader->reads1();

    Slot s = frame->getLocalVars(index);
    if (s.type != PRIMITIVE_INT) {
        jvmAbort("error. slot's type is \n", s.type);  // todo
    }

    s.value.i += value;
    frame->setLocalVars(index, s);
}

#endif
