/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <pthread.h>
#include "thread.h"
#include "../heap/object.h"
#include "../heap/strobj.h"
#include "../../interpreter/interpreter.h"

// Thread specific key holding a thread
static pthread_key_t thread_key;

struct thread *main_thread = NULL;
struct object *system_thread_group = NULL;

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
void create_main_thread(struct classloader *loader)
{
    pthread_key_create(&thread_key, NULL);
    main_thread = thread_create(loader, NULL);

    struct class *jltg_class = load_sys_class("java/lang/ThreadGroup");
    system_thread_group = object_create(jltg_class);

    struct class *jlt_class = load_sys_class("java/lang/Thread");
    struct object *jlt_obj = object_create(jlt_class);

    main_thread->jltobj = jlt_obj;

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
//    struct slot arg = rslot(system_thread_group);
//    thread_invoke_method(main_thread, class_get_constructor(jltg_class, "()V"), &arg);
    class_clinit(jltg_class);

    exec_java_func(class_get_constructor(jltg_class, "()V"), (slot_t *) &system_thread_group);

    // from java/lang/Thread.java
    // public final static int MIN_PRIORITY = 1;
    // public final static int NORM_PRIORITY = 5;
    // public final static int MAX_PRIORITY = 10;
    slot_t value = 5;  // main thread 必须有初始的 priority
//    set_instance_field_value_by_nt(jlt_obj, "priority", "I", &value);
    set_instance_field_value(jlt_obj, class_lookup_field(jlt_class, "priority", "I"), &value);


//    set_thread_self(main_thread);

    // 调用 java/lang/Thread 的构造函数
    struct method *constructor
            = class_get_constructor(jlt_obj->clazz, "(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    slot_t args[] = {
            jlt_obj,
            system_thread_group,
            strobj_create(MAIN_THREAD_NAME) // thread name
    };
//    thread_invoke_method(main_thread, constructor, args);
    class_clinit(jlt_class);
    exec_java_func(constructor, args);
}

struct thread *thread_create(struct classloader *loader, struct object *jltobj)
{
    assert(loader != NULL);

    struct thread *thread = vm_malloc(sizeof( struct thread));
    thread->jltobj = jltobj;
    thread->top_frame = NULL;

//    struct class *jlt_class = classloader_load_class(loader, "java/lang/Thread");
//    thread->this_obj = object_create(jlt_class);
//
//    slot_t s = 1;;  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
//    set_instance_field_value(thread->this_obj, class_lookup_field(jlt_class, "priority", "I"), &s);

//    thread->dyn.bootstrap_method_type = thread->dyn.lookup = thread->dyn.call_set = thread->dyn.exact_method_handle = NULL;

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

    set_thread_self(thread);
    return thread;
}

struct frame *alloc_frame(struct method *m, bool vm_invoke)
{
    struct thread *thread = thread_self();

    struct frame *old_top = thread->top_frame;
    if (old_top == NULL) {
        thread->top_frame = (struct frame *) thread->vm_stack;
    } else {
        thread->top_frame = (struct frame *) ((intptr_t) thread->top_frame + FRAME_SIZE(old_top->method));
    }

    if ((intptr_t) thread->top_frame + FRAME_SIZE(m) - (intptr_t) thread->vm_stack > VM_STACK_SIZE) {
        // todo 栈溢出
        jvm_abort("stack_over_flo");
    }

    struct frame *new_frame = thread->top_frame;
    new_frame->vm_invoke = vm_invoke;
    new_frame->prev = old_top;
    new_frame->stack = new_frame->locals + m->max_locals;
    new_frame->method = m;
    bcr_init(&new_frame->reader, m->code, m->code_length);
    return new_frame;
}

void pop_frame()
{
    struct thread *thread = thread_self();
    assert(thread->top_frame != NULL);
    thread->top_frame = thread->top_frame->prev;
}

int vm_stack_depth()
{
    struct thread *thread = thread_self();
    int depth = 0;
    struct frame *f = thread->top_frame;
    while (f != NULL) {
        depth++;
        f = f->prev;
    }
    return depth;
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

void thread_handle_uncaught_exception(struct object *exception)
{
    assert(exception != NULL);

    struct thread *thread = thread_self();
//    vector_clear(&thread->vm_stack);
    thread->top_frame = NULL; // clear vm_stack
    struct method *pst = class_lookup_instance_method(exception->clazz, "printStackTrace", "()V");
    exec_java_func(pst, (slot_t *) &exception);
}

_Noreturn void thread_throw_null_pointer_exception()
{
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_negative_array_size_exception(int array_size)
{
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_array_index_out_of_bounds_exception(int index)
{
    // todo
    jvm_abort("");
}

_Noreturn void thread_throw_class_cast_exception(const char *from_class_name, const char *to_class_name)
{
    assert(from_class_name != NULL);
    assert(to_class_name != NULL);
    // todo
    jvm_abort("");
}

void thread_destroy(struct thread *thread)
{
    // todo
    assert(thread != NULL);
//    vector_release(&thread->vm_stack);
    free(thread);
}
