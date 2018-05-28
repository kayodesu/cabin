/*
 * Author: Jia Yang
 */

#ifndef JVM_CONVERSIONS_H
#define JVM_CONVERSIONS_H

#include "../interpreter/StackFrame.h"


template <typename SlotType, void (SlotType::*convert)()>
static void x2x(StackFrame *frame) {
    Slot *s = frame->operandStack.peek();
    if (typeid(*s) == typeid(SlotType)) {
//        jvmAbort("error. src = %d, curr = %d\n", src, type);
        jvmAbort("error.\n");
    } else {
        (dynamic_cast<SlotType *>(s)->*convert)();
    }
}

#endif //JVM_CONVERSIONS_H
