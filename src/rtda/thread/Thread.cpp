/*
 * Author: kayo
 */

#include <pthread.h>
#include "../../debug.h"
#include "Thread.h"
#include "../heap/Object.h"
#include "../../interpreter/interpreter.h"
#include "../heap/StringObject.h"

#if TRACE_THREAD
#define TRACE PRINT_TRACE
#else
#define TRACE(x)
#endif

// Thread specific key holding a Thread
static pthread_key_t thread_key;

Thread *thread_self()
{
    return (Thread *) pthread_getspecific(thread_key);
}

static inline void set_thread_self(Thread *thread)
{
    pthread_setspecific(thread_key, thread);
}

void init_thread_module()
{
    pthread_key_create(&thread_key, nullptr);
}

Thread::Thread(Object *jltobj, Object *threadGroup, const char *threadName, int priority)
{
    assert(MIN_PRIORITY <= priority && priority <= MAX_PRIORITY);

    set_thread_self(this);
    vmEnv.threads.push_back(this);

    Class *jlt_class = loadSysClass(S(java_lang_Thread));
    jlt_class->clinit();

    if (jltobj != nullptr)
        this->jltobj = jltobj;
    else
        this->jltobj = Object::newInst(jlt_class);
    this->jltobj->setInstFieldValue(S(priority), S(I), (slot_t *) &priority);

    if (threadGroup != nullptr)
        setThreadGroupAndName(threadGroup, threadName);
}

void Thread::setThreadGroupAndName(Object *threadGroup, const char *threadName)
{
    assert(threadGroup != nullptr);

    if (threadName == nullptr) {
        // todo
        threadName = "unknown";
    }

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = jltobj->clazz->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
//    slot_t args[] = {
//            (slot_t) jltobj,
//            (slot_t) threadGroup,
//            (slot_t) StringObject::newInst(threadName) // thread name
//    };
    execJavaFunc(constructor, { (slot_t) jltobj,
                                (slot_t) threadGroup,
                                (slot_t) StringObject::newInst(threadName) });
}

bool Thread::isAlive() {
    assert(jltobj != nullptr);
    auto status = jltobj->getInstFieldValue<jint>("threadStatus", "I");
    // todo
    return false;
}

Frame *allocFrame(Method *m, bool vm_invoke)
{
    Thread *thread = thread_self();

    Frame *old_top = thread->topFrame;
    if (old_top == nullptr) {
        thread->topFrame = (Frame *) thread->vmStack;
    } else {
        thread->topFrame = (Frame *) ((intptr_t) thread->topFrame + Frame::size(old_top->method));
    }

    if ((intptr_t) thread->topFrame + Frame::size(m) - (intptr_t) thread->vmStack > VM_STACK_SIZE) {
        // todo 栈溢出
        jvm_abort("stack_over_flo");
    }

    return new(thread->topFrame) Frame(m, vm_invoke, old_top);
}

void popFrame()
{
    Thread *thread = thread_self();
    assert(thread->topFrame != nullptr);
    thread->topFrame = thread->topFrame->prev;
}

int vm_stack_depth()
{
    Thread *thread = thread_self();
    int depth = 0;
    Frame *f = thread->topFrame;
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
    thread->topFrame = nullptr; // clear vm_stack
    Method *pst = exception->clazz->lookupInstMethod(S(printStackTrace), S(___V));
    execJavaFunc(pst, exception);
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
