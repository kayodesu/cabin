/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "stack_frame.h"

struct stack_frame* sf_create(struct jthread *thread, struct jmethod *method)
{
    assert(thread != NULL);
    assert(method != NULL);

    VM_MALLOC(struct stack_frame, frame);

    frame->thread = thread;
    frame->method = method;
    frame->operand_stack = os_create(method->max_stack);
    frame->max_locals = method->max_locals;
    frame->local_vars = malloc(sizeof(struct slot) * frame->max_locals);
    frame->reader = bcr_create(method->code, method->code_length);
    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;

//    for (int i = 0; i < maxLocals; i++) {
//        localVars[i]->type = INVALID_JVM_TYPE;  // todo
//    }

//    // 准备参数
//    for (int i = method->getArgSlotCount() - 1; i >= 0; i--) {
//        setLocalVars(i, operandStack.popSlot());
//    }

    return frame;
}

void sf_set_local_var(struct stack_frame *frame, int index, const struct slot *value)
{
    assert(frame != NULL);
    assert(index >= 0 && index < frame->max_locals);

    frame->local_vars[index] = *value;
    if (slot_is_category_two(value)) {
        frame->local_vars[++index] = phslot;
    }
}

//void sf_invoke_method(struct stack_frame *frame, struct jmethod *method, const struct slot *args)
//{
//    jvm_abort("error. 调用 jthread_invoke_method 函数");
//    assert(frame != NULL && method != NULL);
////    if (method->isNative()) {  //todo
////        auto nativeMethod = findNativeMethod(method->jclass->className, method->name, method->descriptor);
////        if (nativeMethod == nullptr) {
////            jvmAbort("error. not find native function: %s, %s, %s\n",
////                       method->jclass->className.c_str(), method->name.c_str(), method->descriptor.c_str());    // todo
////        }
////        nativeMethod(this);
////        return;
////    }
//
//    struct stack_frame *new_frame = sf_create(frame->thread, method);
//    jthread_push_frame(frame->thread, new_frame);
//
//    // 准备参数
//    if (args != NULL) {
//        for (int i = 0; i < method->arg_slot_count; i++) {
//            sf_set_local_var(new_frame, i, &args[i]);  // todo 啥意思
//        }
//    } else {
//        for (int i = method->arg_slot_count - 1; i >= 0; i--) {
//            sf_set_local_var(new_frame, i, os_pops(frame->operand_stack));  // todo 啥意思
//        }
//    }
//
//    // 中断 invokeFrame 的执行，执行 new_frame
//    sf_interrupt(frame);
//
//    // todo
//}

void sf_destroy(struct stack_frame *frame)
{
    if (frame == NULL) {
        // todo
        return;
    }

    // todo
    free(frame);
}

char* sf_to_string(const struct stack_frame *frame)
{
    return jmethod_to_string(frame->method); // todo
}
