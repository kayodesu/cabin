/*
 * Author: Jia Yang
 */

#ifndef JVM_COMPARISONS_H
#define JVM_COMPARISONS_H

#include "../interpreter/StackFrame.h"
#include "../rtda/thread/Jthread.h"
#include "../rtda/heap/methodarea/Jmethod.h"

template <typename T, T (OperandStack::*pop)(), bool (*cmp)(T, T), bool cmpWith0 = false>
static void ifCmpCond(StackFrame *frame) {
    T value2 = 0;
    if (!cmpWith0) {
        value2 = (frame->operandStack.*pop)();
    }
    T value1 = (frame->operandStack.*pop)();
    int offset = frame->reader->reads2();
    if (cmp(value1, value2)) {
        frame->reader->skip(offset - 3); // 减3？减去本条指令自身的长度
    }
}

#endif
