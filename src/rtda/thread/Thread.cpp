/*
 * Author: kayo
 */

#include <pthread.h>
#include "Thread.h"
#include "../heap/Object.h"
#include "../../interpreter/interpreter.h"
#include "../../symbol.h"
#include "../heap/StringObject.h"

// Thread specific key holding a Thread
static pthread_key_t thread_key;

Thread *main_thread = nullptr;
Object *system_thread_group = nullptr;

Thread *thread_self()
{
    return (Thread *) pthread_getspecific(thread_key);
}

static inline void set_thread_self(Thread *thread)
{
    pthread_setspecific(thread_key, thread);
}

/*
 * main thread 由虚拟机启动。
 */
void createMainThread(ClassLoader *loader)
{
    pthread_key_create(&thread_key, nullptr);
    main_thread = new Thread(loader, nullptr);

    Class *jltg_class = loadSysClass(S(java_lang_ThreadGroup));
    system_thread_group = Object::newInst(jltg_class);

    Class *jlt_class = loadSysClass(S(java_lang_Thread));
    Object *jlt_obj = Object::newInst(jlt_class);

    main_thread->jltobj = jlt_obj;

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    jltg_class->clinit();

    execJavaFunc(jltg_class->getConstructor(S(___V)), (slot_t *) &system_thread_group);

    // from java/lang/Thread.java
    // public final static int MIN_PRIORITY = 1;
    // public final static int NORM_PRIORITY = 5;
    // public final static int MAX_PRIORITY = 10;
    slot_t value = 5;  // main thread 必须有初始的 priority
    jlt_obj->setInstFieldValue(S(priority), S(I), &value);

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = jlt_obj->clazz->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");

    jlt_class->clinit();

    slot_t args[] = {
            (slot_t) jlt_obj,
            (slot_t) system_thread_group,
            (slot_t) StringObject::newInst(MAIN_THREAD_NAME) // thread name
    };
    execJavaFunc(constructor, args);
}

Thread::Thread(ClassLoader *loader, Object *jltobj): jltobj(jltobj)
{
    assert(loader != nullptr);

//    Class *jlt_class = classloader_load_class(loader, "java/lang/Thread");
//    thread->this_obj = object_create(jlt_class);
//
//    slot_t s = 1;;  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
//    set_instance_field_value(thread->this_obj, class_lookup_field(jlt_class, "priority", "I"), &s);

//    Thread->dyn.bootstrap_method_type = Thread->dyn.lookup = Thread->dyn.call_set = Thread->dyn.exact_method_handle = NULL;

    /*
    auto jlThreadClass = classLoader->loadClass("java/lang/Thread");
    jlThreadObj = new Jobject(jlThreadClass);
    Jvalue v;
    v.i = 1;  // todo. why 1? I do not know. 参见 jvmgo/instructions/reserved/bootstrap.go
    jlThreadObj->setInstanceFieldValue("priority", "I", v);

    // 置此线程的 ThreadGroup
    if (mainThreadGroup != nullptr) {  // todo
//        auto constructor = jlThreadClass->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
//        auto Frame = new StackFrame(this, constructor);
//        Frame->operandStack.push(jlThreadObj);
//        assert(mainThreadGroup != nullptr);
//        Frame->operandStack.push(mainThreadGroup);
//        JstringObj *o = new JstringObj(classLoader, strToJstr("main"));
//        Frame->operandStack.push(o);
//
//        pushFrame(Frame);
//        interpret(this);
//
//        delete o;
        joinToMainThreadGroup();
    }
     */

    set_thread_self(this);
}

Frame *allocFrame(Method *m, bool vm_invoke)
{
    Thread *thread = thread_self();

    Frame *old_top = thread->top_frame;
    if (old_top == nullptr) {
        thread->top_frame = (Frame *) thread->vm_stack;
    } else {
        thread->top_frame = (Frame *) ((intptr_t) thread->top_frame + Frame::size(old_top->method));
    }

    if ((intptr_t) thread->top_frame + Frame::size(m) - (intptr_t) thread->vm_stack > VM_STACK_SIZE) {
        // todo 栈溢出
        jvm_abort("stack_over_flo");
    }

    return new(thread->top_frame) Frame(m, vm_invoke, old_top);
}

void popFrame()
{
    Thread *thread = thread_self();
    assert(thread->top_frame != nullptr);
    thread->top_frame = thread->top_frame->prev;
}

int vm_stack_depth()
{
    Thread *thread = thread_self();
    int depth = 0;
    Frame *f = thread->top_frame;
    while (f != nullptr) {
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

void thread_handle_uncaught_exception(Object *exception)
{
    assert(exception != nullptr);

    Thread *thread = thread_self();
    thread->top_frame = nullptr; // clear vm_stack
    Method *pst = exception->clazz->lookupInstMethod(S(printStackTrace), S(___V));
    execJavaFunc(pst, (slot_t *) &exception);
}

void thread_throw_null_pointer_exception()
{
    // todo
    jvm_abort("null_pointer_exception");
}

void thread_throw_negative_array_size_exception(int array_size)
{
    // todo
    jvm_abort("negative_array_size_exception");
}

void thread_throw_array_index_out_of_bounds_exception(int index)
{
    // todo
    jvm_abort("array_index_out_of_bounds_exception");
}

void thread_throw_class_cast_exception(const char *from_class_name, const char *to_class_name)
{
    assert(from_class_name != nullptr);
    assert(to_class_name != nullptr);
    // todo
    jvm_abort("thread_throw_class_cast_exception");
}
