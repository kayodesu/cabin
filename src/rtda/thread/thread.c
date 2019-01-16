/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <pthread.h>
#include "thread.h"
#include "../heap/object.h"
#include "../heap/strobj.h"

#if 0
// Thread specific key holding a thread
static pthread_key_t thread_key;

struct thread *main_thread = NULL;

struct thread* thread_self()
{
    return (struct thread *) pthread_getspecific(thread_key);
}

static inline void set_thread_self(struct thread *thread)
{
    pthread_setspecific(thread_key, thread);
}

/*
 * main thread 由虚拟机启动。
 */
static void create_main_thread(struct classloader *loader)
{
    pthread_key_create(&thread_key, NULL);

    struct class *jltg_class = load_sys_class("java/lang/ThreadGroup");
    system_thread_group = object_create(jltg_class);

    struct class *jlt_class = load_sys_class("java/lang/Thread");
    struct object *jlt_obj = object_create(jlt_class);

    // from java/lang/Thread.java
    // public final static int MIN_PRIORITY = 1;
    // public final static int NORM_PRIORITY = 5;
    // public final static int MAX_PRIORITY = 10;
    struct slot value = islot(5);  // todo. why set this? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    set_instance_field_value_by_nt(jlt_obj, "priority", "I", &value);

    main_thread = thread_create(loader, jlt_obj);
    set_thread_self(main_thread);

    // 调用 java/lang/Thread 的构造函数
    struct method *constructor
            = class_get_constructor(jlt_obj->clazz, "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    struct slot args[] = {
            rslot(jlt_obj),
            rslot(system_thread_group),
            rslot(strobj_create(MAIN_THREAD_NAME)) // thread name
    };
    thread_invoke_method(main_thread, constructor, args);
    class_clinit(jlt_obj->clazz, main_thread); // 最后压栈，保证先执行。


    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    struct slot arg = rslot(system_thread_group);
    thread_invoke_method(main_thread, class_get_constructor(jltg_class, "()V"), &arg);
    class_clinit(jltg_class, main_thread);

    // 现在，main thread's vm stack 里面按将要执行的顺序（从栈顶到栈底）分别为：
    // java/lang/ThreadGroup~<clinit>
    // java/lang/ThreadGroup~<init>~()V
    // java/lang/Thread~<clinit>
    // java/lang/Thread~<init>~(Ljava/lang/ThreadGroup;Ljava/lang/String;)V
    interpret(main_thread);
}
#endif

struct thread* thread_create(struct classloader *loader, struct object *jltobj)
{
    assert(loader != NULL);

    VM_MALLOC(struct thread, thread);

    vector_init(&thread->vm_stack);
    thread->jltobj = jltobj;
    for (int i = 0; i < FRMHUB_SLOTS_COUNT_MAX; i++) {
        vector_init(thread->frame_cache + i);
    }

//    struct class *jlt_class = classloader_load_class(loader, "java/lang/Thread");
//    thread->this_obj = jobject_create(jlt_class);

//    struct slot value = islot(1);  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
//    set_instance_field_value_by_nt(thread->this_obj, "priority", "I", &value);

    thread->dyn.bootstrap_method_type = thread->dyn.lookup = thread->dyn.call_set = thread->dyn.exact_method_handle = NULL;

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

bool thread_is_stack_empty(const struct thread *thread)
{
    assert(thread != NULL);
    return vector_len(&thread->vm_stack) == 0;
}

int thread_stack_depth(const struct thread *thread)
{
    assert(thread != NULL);
    return vector_len(&thread->vm_stack);
}

struct frame* thread_top_frame(struct thread *thread)
{
    assert(thread != NULL);
    return vector_back(&thread->vm_stack);
}

//struct frame* jthread_depth_frame(struct thread *thread, int depth)
//{
//    assert(thread != NULL);
//    return vector_rget(&thread->vm_stack, depth);
//}

struct frame* thread_stack_frame_from_top(struct thread *thread, int from_top)
{
    assert(thread != NULL);
    return vector_rget(&thread->vm_stack, from_top);
}


struct frame* thread_pop_frame(struct thread *thread)
{
    assert(thread != NULL);
    return vector_pop_back(&thread->vm_stack);
}

static void thread_push_frame(struct thread *thread, struct frame *frame)
{
    assert(thread != NULL && frame != NULL);
    vector_push_back(&thread->vm_stack, frame);
}

struct frame** thread_get_frames(const struct thread *thread, int *num)
{
    assert(thread != NULL);
    assert(num != NULL);
    return (struct frame **) vector_to_array(&thread->vm_stack, num);
}

static struct frame* frame_cache_get(struct thread *thread, struct method *m)
{
    assert(thread != NULL);
    assert(m != NULL);

    u4 max_locals_stack = m->max_locals + m->max_stack;
    if (max_locals_stack < FRMHUB_SLOTS_COUNT_MAX) {
        struct vector *v = thread->frame_cache + max_locals_stack;
        if (!vector_empty(v)) {
            struct frame *f = vector_pop_back(v);
            frame_bind(f, thread, m);
            return f;
        }
    }

    return frame_create(thread, m);
}

void thread_recycle_frame(struct frame *frame)
{
    assert(frame != NULL);

    if (frame->max_locals_and_stack >= FRMHUB_SLOTS_COUNT_MAX) {
        frame_destroy(frame); // too big, don't accept
    } else {
        vector_push_back(frame->thread->frame_cache + frame->max_locals_and_stack, frame);
    }
}

void thread_invoke_method(struct thread *thread, struct method *method, const struct slot *args)
{
    assert(thread != NULL && method != NULL);

    struct frame *top_frame = thread_top_frame(thread);
    if (top_frame == NULL) {
        // todo 没有调用者，那么是每个线程的启动函数（主线程的启动函数就是main）
    }

    struct frame *new_frame = frame_cache_get(thread, method); // frame_create(thread, method);
    thread_push_frame(thread, new_frame);

    if (method->arg_slot_count > 0 && args == NULL) {
        jvm_abort("do not find args, %d\n", method->arg_slot_count); // todo
    }

    // 准备参数
    for (int i = 0; i < method->arg_slot_count; i++) {
        // 传递参数到被调用的函数。
        frame_locals_set(new_frame, i, args + i);
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
        frame_interrupt(top_frame);
    }

    // todo
}

void thread_invoke_method_with_shim(struct thread *thread, struct method *method, const struct slot *args,
                                     void (* shim_action)(struct frame *))
{
    thread_invoke_method(thread, method, args);

    struct frame *top = thread_top_frame(thread);
    thread_pop_frame(thread);

    // 创建一个 shim stack frame 来接受函数method的返回值
    thread_push_frame(thread, frame_create(thread, shim_action));
    thread_push_frame(thread, top);
}

void thread_handle_uncaught_exception(struct thread *thread, struct object *exception)
{
    assert(thread != NULL);
    assert(exception != NULL);

    vector_clear(&thread->vm_stack);
    struct method *pst = class_lookup_instance_method(exception->clazz, "printStackTrace", "()V");

    // call exception.printStackTrace()
    struct frame *frame = frame_cache_get(thread, pst);//frame_create(thread, pst);
    frame->locals[0] = rslot(exception);
    thread_push_frame(thread, frame);
}

_Noreturn void thread_throw_null_pointer_exception(struct thread *thread)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_negative_array_size_exception(struct thread *thread, int array_size)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_array_index_out_of_bounds_exception(struct thread *thread, int index)
{
    assert(thread != NULL);
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_class_cast_exception(
        struct thread *thread, const char *from_class_name, const char *to_class_name)
{
    assert(thread != NULL);
    assert(from_class_name != NULL);
    assert(to_class_name != NULL);
    // ("%s can not be cast to %s\n", from_class_name, to_class_name);
    // todo
    jvm_abort("");
}

void thread_destroy(struct thread *thread)
{
    // todo
    assert(thread != NULL);
    vector_release(&thread->vm_stack);
    free(thread);
}
