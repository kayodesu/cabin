/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jthread.h"
#include "../heap/jobject.h"
#include "../../util/vector.h"

struct jthread {
    struct vector *vm_stack; // 虚拟机栈，一个线程只有一个虚拟机栈
    struct jobject *jlt_obj; // object of java.lang.Thread   todo 干嘛用的

    size_t pc;
};

struct jthread* jthread_create()
{
    VM_MALLOC(struct jthread, thread);

    thread->vm_stack = vector_create();

    return thread;
}

void jthread_set_pc(struct jthread *thread, size_t new_pc)
{
    assert(thread != NULL);
    thread->pc = new_pc;
}

size_t jthread_get_pc(const struct jthread *thread)
{
    assert(thread != NULL);
    return thread->pc;
}

bool jthread_is_stack_empty(const struct jthread *thread)
{
    assert(thread != NULL);
    return vector_len(thread->vm_stack) == 0;
}

struct stack_frame* jthread_top_frame(struct jthread *thread)
{
    assert(thread != NULL);
    return vector_back(thread->vm_stack);
}

void jthread_pop_frame(struct jthread *thread)
{
    assert(thread != NULL);
    vector_pop_back(thread->vm_stack);
}

void jthread_push_frame(struct jthread *thread, struct stack_frame *frame)
{
    assert(thread != NULL && frame != NULL);
    vector_push_back(thread->vm_stack, frame);
}

void jthread_invoke_method(struct jthread *thread, struct jmethod *method, const struct slot *args)
{
    assert(thread != NULL && method != NULL);

    struct stack_frame *top_frame = jthread_top_frame(thread);
    if (top_frame == NULL) {
        // todo 没有调用者，那么是每个线程的启动函数（主线程的启动函数就是main）
    }

    struct stack_frame *new_frame = sf_create(thread, method);
    jthread_push_frame(thread, new_frame);

    // 准备参数
    if (args != NULL) {
        for (int i = 0; i < method->arg_slot_count; i++) {
            // 传递参数到被调用的函数。
            sf_set_local_var(new_frame, i, &args[i]);
        }
    } else {
        for (int i = method->arg_slot_count - 1; i >= 0; i--) {
            assert(top_frame != NULL);
            sf_set_local_var(new_frame, i, os_pops(top_frame->operand_stack));
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
    assert(thread != NULL);
    vector_destroy(thread->vm_stack);
    free(thread);
}
