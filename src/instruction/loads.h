/*
 * Author: Jia Yang
 */

#ifndef JVM_LOADS_H
#define JVM_LOADS_H

#include <functional>
#include "../jvm.h"
#include "../interpreter/StackFrame.h"
#include "../rtda/heap/objectarea/Jobject.h"
#include "../rtda/heap/objectarea/JarrayObj.h"

template <Jtype type, int index = -1>
static void __tload(StackFrame *frame) {
    int i = index;
    if (i < 0) {
        i = frame->reader->readu1();
    }
    const Slot &s = frame->getLocalVars(i);
    if (s.type != type) {
        /* todo throw new BytecodeFormatError("类型不匹配");*/
        jvmAbort("error. %d, %d\n", s.type, type);
    }

    frame->operandStack.push(s);
}

template <bool (Jclass::*checkType)() const, typename SaveType>
static void __taload(StackFrame *frame) {
    jint index = frame->operandStack.popInt();
    jreference r = frame->operandStack.popReference();

    if (r == nullptr) {
        jvmAbort("error NULL Point Exception\n"); // todo
    }

    auto a = static_cast<JarrayObj *>(r);

    if (!a->getClass()->isArray()) {
        jvmAbort("error. %s\n", a->getClass()->className.c_str()); // todo
    }

    if (!a->checkBounds(index)) {
        jvmAbort("ArrayIndexOutOfBoundsException, index = %d out of [0, %d)\n", index, a->length());
        /* todo throw new ArrayIndexOutOfBoundsException(String.valueOf(index)); */
    }

    if (!(a->getClass()->*checkType)()) {
        jvmAbort("error\n");
        /* todo  */
    }

    SaveType v = a->get<SaveType>(index);

    frame->operandStack.push(a->get<SaveType>(index));  // todo

//    if (is_one_dimension_reference_array(arr->class)) {
//        operand_stack_push_reference(frame, JARRAY_DATA(arr, void *)[index]);
//    } else if (is_multi_array(arr->class)) {
//        operand_stack_push_reference(frame, JMULTIARRAY_DATA(arr, void *)[index]); // todo JMULTIARRAY_DATA
//    } else {
//        jprintf("error\n");
//    }
}

#endif
