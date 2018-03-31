/*
 * Author: Jia Yang
 */

#ifndef JVM_EXTENDED_H
#define JVM_EXTENDED_H

#include "../interpreter/StackFrame.h"

static void multianewarray(StackFrame *frame) {
    int index = frame->reader->readu2();
    const string &className = frame->method->jclass->rtcp->getClassName(index);
    Jclass *arr_class = resolveClass(frame->method->jclass, className);

    jint arr_dim = frame->reader->readu1();
    jint arr_lens[arr_dim];
    for (jint i = arr_dim - 1; i >= 0; i--) {
        arr_lens[i] = frame->operandStack.popInt();
        if (arr_lens[i] < 0) {
            // todo  java.lang.NegativeArraySizeException
            jvmAbort("error. java.lang.NegativeArraySizeException \n");
            return;
        }
    }

    frame->operandStack.push(new JarrayObj(arr_class, arr_dim, arr_lens));
}

static void ifnull(StackFrame *frame) {
    auto offset = frame->reader->readu2();

    if (frame->operandStack.popReference() == nullptr) {
        frame->reader->skip(offset - 3); // 减3？减去本条指令自身的长度
    }
}

static void ifnonnull(StackFrame *frame) {
    auto offset =frame->reader->readu2();

    if (frame->operandStack.popReference() != nullptr) {
        frame->reader->skip(offset - 3); // 减3？减去本条指令自身的长度
    }
}

#endif //JVM_EXTENDED_H
