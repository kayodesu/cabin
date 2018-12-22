/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <pthread.h>
#include "jthread.h"
#include "../heap/jobject.h"
#include "../../util/vector.h"
#include "../../interpreter/interpreter.h"


struct jthread* jthread_create(struct classloader *loader)
{
    VM_MALLOC(struct jthread, thread);

    thread->vm_stack = vector_create();

    struct jclass *jlt_class = classloader_load_class(loader, "java/lang/Thread");
    thread->this_obj = jobject_create(jlt_class);

    struct slot value = islot(1);  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    set_instance_field_value_by_nt(thread->this_obj, "priority", "I", &value);

    thread->dyn.caller = thread->dyn.invoked_type = thread->dyn.call_set = thread->dyn.exact_method_handle = NULL;

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

//    pthread_create(NULL, NULL, NULL, NULL); // todo

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

//void jthread_set_pc(struct jthread *thread, size_t new_pc)
//{
//    assert(thread != NULL);
//    thread->pc = new_pc;
//}
//
//size_t jthread_get_pc(const struct jthread *thread)
//{
//    assert(thread != NULL);
//    return thread->pc;
//}

bool jthread_is_stack_empty(const struct jthread *thread)
{
    assert(thread != NULL);
    return vector_len(thread->vm_stack) == 0;
}

int jthread_stack_depth(const struct jthread *thread)
{
    assert(thread != NULL);
    return vector_len(thread->vm_stack);
}

struct stack_frame* jthread_top_frame(struct jthread *thread)
{
    assert(thread != NULL);
    return vector_back(thread->vm_stack);
}

struct stack_frame* jthread_depth_frame(struct jthread *thread, int depth)
{
    assert(thread != NULL);
    return vector_rget(thread->vm_stack, depth);
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

struct stack_frame** jthread_get_frames(const struct jthread *thread, int *num)
{
    assert(thread != NULL);
    assert(num != NULL);
    return (struct stack_frame **) vector_to_array(thread->vm_stack, num);
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

    if (method->arg_slot_count > 0 && args == NULL) {
        jvm_abort("do not find args, %d\n", method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        sf_set_local_var(new_frame, i, args + i);
    }

//    // 准备参数
//    if (args != NULL) {
//        for (int i = 0; i < method->arg_slot_count; i++) {
//            // 传递参数到被调用的函数。
//            sf_set_local_var(new_frame, i, &args[i]);
//        }
//    } else {
//        for (int i = method->arg_slot_count - 1; i >= 0; i--) {
//            assert(top_frame != NULL);
//            sf_set_local_var(new_frame, i, os_pops(top_frame->operand_stack));
//        }
//    }

    // 中断 top_frame 的执行，执行 new_frame
    if (top_frame != NULL) {
        sf_interrupt(top_frame);
    }

    // todo
}

void jthread_invoke_method_with_shim(struct jthread *thread, struct jmethod *method, const struct slot *args,
                                     void (* shim_action)(struct stack_frame *))
{
    jthread_invoke_method(thread, method, args);

    struct stack_frame *top = jthread_top_frame(thread);
    jthread_pop_frame(thread);

    // 创建一个 shim stack frame 来接受函数method的返回值
    jthread_push_frame(thread, sf_create_shim(thread, shim_action));
    jthread_push_frame(thread, top);
}

void jthread_handle_uncaught_exception(struct jthread *thread, struct jobject *exception)
{
    assert(thread != NULL);
    assert(exception != NULL);

    vector_clear(thread->vm_stack);
    struct jmethod *pst = jclass_lookup_instance_method(exception->jclass, "printStackTrace", "()V");

    // call exception.printStackTrace()
    struct stack_frame *frame = sf_create(thread, pst);
    frame->local_vars[0] = rslot(exception);
    jthread_push_frame(thread, frame);
}

void jthread_throw_null_pointer_exception(struct jthread *thread)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

void jthread_throw_negative_array_size_exception(struct jthread *thread, int array_size)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

void jthread_throw_array_index_out_of_bounds_exception(struct jthread *thread, int index)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

void jthread_throw_class_cast_exception(
        struct jthread *thread, const char *from_class_name, const char *to_class_name)
{
    assert(thread != NULL);
    assert(from_class_name != NULL);
    assert(to_class_name != NULL);
    // ("%s can not be cast to %s\n", from_class_name, to_class_name);
    // todo
    jvm_abort("");
}

void jthread_destroy(struct jthread *thread)
{
    // todo
    assert(thread != NULL);
    vector_destroy(thread->vm_stack);
    free(thread);
}
