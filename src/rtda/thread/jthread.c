/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jthread.h"
#include "../heap/jobject.h"
#include "../../util/vector.h"
#include "../../interpreter/interpreter.h"

struct jthread {
    struct vector *vm_stack; // 虚拟机栈，一个线程只有一个虚拟机栈
    struct jobject *this_obj; // object of java.lang.Thread   todo 干嘛用的

    size_t pc;
};

struct jthread* jthread_create(struct classloader *loader)
{
    VM_MALLOC(struct jthread, thread);

    thread->vm_stack = vector_create();

    struct jclass *jlt_class = classloader_load_class(loader, "java/lang/Thread");
    thread->this_obj = jobject_create(jlt_class);

    struct slot value = islot(1);  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    set_instance_field_value_by_nt(thread->this_obj, "priority", "I", &value);

    /*
    auto jlThreadClass = classLoader->loadClass("java/lang/Thread");
    jlThreadObj = new Jobject(jlThreadClass);
    Jvalue v;
    v.i = 1;  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    jlThreadObj->setInstanceFieldValue("priority", "I", v);

    // 置此线程的 ThreadGroup
    if (mainThreadGroup != nullptr) {  // todo
//        auto constructor = jlThreadClass->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
//        auto frame = new StackFrame(this, constructor);
//        frame->operandStack.push(jlThreadObj);
//        assert(mainThreadGroup != nullptr);
//        frame->operandStack.push(mainThreadGroup);
//        JstringObj *o = new JstringObj(classLoader, strToJstr("main"));
//        frame->operandStack.push(o);
//
//        pushFrame(frame);
//        interpret(this);
//
//        delete o;
        joinToMainThreadGroup();
    }
     */

    return thread;
}


#if 0

static void JThread::joinToMainThreadGroup() {
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

struct jobject* jthread_get_obj(struct jthread *thread)
{
    assert(thread != NULL);
    return thread->this_obj;
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

void jthread_destroy(struct jthread *thread)
{
    // todo
    assert(thread != NULL);
    vector_destroy(thread->vm_stack);
    free(thread);
}
