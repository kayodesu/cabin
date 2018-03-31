/*
 * Author: Jia Yang
 */

#ifndef JVM_CONTROLS_H
#define JVM_CONTROLS_H

#include "../interpreter/StackFrame.h"
#include "../rtda/thread/Jthread.h"

static void __goto(StackFrame *frame) {
    int offset = frame->reader->reads2();
    frame->reader->skip(offset- 3);  // 减3？减去本条指令自身的长度
}

template <typename T = jint, T (OperandStack::*pop)() = nullptr>
static void __return(StackFrame *frame) {
//    frame->method->reset();
    frame->thread->popFrame();

    if(pop != nullptr) {
        StackFrame *invokeFrame = frame->thread->topFrame();
        // todo invokeFrame 会不会为null
        invokeFrame->operandStack.push((frame->operandStack.*pop)());
    }

    frame->exeOver();
}

#endif
