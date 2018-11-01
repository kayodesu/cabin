/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jthread.h"
//#include "../../interpreter/stack_frame.h"

struct jthread* jthread_create()
{
    VM_MALLOC(struct jthread, thread);

    utarray_new(thread->vm_stack, &ut_ptr_icd);

    return thread;
}

void jthread_push_frame(struct jthread *thread, struct stack_frame *frame)
{
    assert(thread != NULL && frame != NULL);
    utarray_push_back(thread->vm_stack, &frame);
}

void jthread_invoke_method(struct jthread *thread, struct jmethod *method, const struct slot *args)
{
    assert(thread != NULL && method != NULL);
//    if (method->isNative()) {  //todo
//        auto nativeMethod = findNativeMethod(method->jclass->className, method->name, method->descriptor);
//        if (nativeMethod == nullptr) {
//            jvmAbort("error. not find native function: %s, %s, %s\n",
//                       method->jclass->className.c_str(), method->name.c_str(), method->descriptor.c_str());    // todo
//        }
//        nativeMethod(this);
//        return;
//    }

    struct stack_frame *top_frame = jthread_top_frame(thread);
    if (top_frame == NULL) {
        // todo
    }

    struct stack_frame *new_frame = sf_create(thread, method);
    jthread_push_frame(thread, new_frame);

    // 准备参数
    if (args != NULL) {
        for (int i = 0; i < method->arg_slot_count; i++) {
            sf_set_local_var(new_frame, i, &args[i]);  // todo 啥意思
        }
    } else {
        if (method->arg_slot_count > 0 && top_frame == NULL) {  // todo
            jvm_abort("error\n");
        }
        for (int i = method->arg_slot_count - 1; i >= 0; i--) {
            sf_set_local_var(new_frame, i, os_pops(top_frame->operand_stack));  // todo 啥意思
        }
    }

    // 中断 top_frame 的执行，执行 new_frame
    if (top_frame != NULL) {
        sf_interrupt(top_frame);
    }

    // todo
}

#if 0

void JThread::joinToMainThreadGroup() {
    assert(mainThreadGroup != nullptr);

    auto jlThreadClass = classLoader->loadClass("java/lang/Thread");
    auto constructor = jlThreadClass->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    auto frame = new StackFrame(this, constructor);

//    frame->operandStack.push(jlThreadObj);
//
//    frame->operandStack.push(mainThreadGroup);
    JStringObj *o = JStringObj::newJStringObj(classLoader, strToJstr("main"));
//    frame->operandStack.push(o);

    frame->setLocalVar(0, Slot(jlThreadObj));
    frame->setLocalVar(1, Slot(mainThreadGroup));
    frame->setLocalVar(2, Slot(o));

    pushFrame(frame);
    interpret(this);

    // 无需在这里 delete frame, interpret函数会delete调执行过的frame

    delete o;
}

#endif

void jthread_destroy(struct jthread *thread)
{
    // todo
}
