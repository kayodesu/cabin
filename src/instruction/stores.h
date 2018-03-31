/*
 * Author: Jia Yang
 */

#ifndef JVM_STORES_H
#define JVM_STORES_H

#include "../interpreter/StackFrame.h"
#include "../rtda/heap/objectarea/Jobject.h"
#include "../rtda/heap/objectarea/JarrayObj.h"


template <Jtype type, int index = -1>
static void __tstore(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot s = os.popSlotJumpInvalid();
    if (s.type != type) {
        /* todo */
        jvmAbort("error, type mismatch. %d, %d\n", s.type, type);
    }

    int i = index;
    if (i < 0) {
        i = frame->reader->readu1();
    }
    frame->setLocalVars(i, s);
}

template <typename T, T (OperandStack::*popValue)(), bool (Jclass::*checkType)() const, typename SaveType>
static void __tastore(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    T value = (os.*popValue)();

    jint index = os.popInt();
    jreference r = os.popReference();
    if (r == nullptr) {
        jvmAbort("error NULL Point Exception\n"); // todo
    }

    JarrayObj *a = static_cast<JarrayObj *>(r);

    if (!a->checkBounds(index)) {
        jvmAbort("ArrayIndexOutOfBoundsException\n");
        /* todo throw new ArrayIndexOutOfBoundsException(String.valueOf(index)); */
    }

    if (!(a->getClass()->*checkType)()) {
        jvmAbort("error\n");
    }

    a->set<SaveType>(index, (SaveType)value); // todo
}

#endif
