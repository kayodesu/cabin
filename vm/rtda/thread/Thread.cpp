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

static Field *eetopField;
static Method *runMethod;

Thread *initMainThread()
{
    pthread_key_create(&thread_key, nullptr);

    eetopField = java_lang_Thread_class->lookupInstField("eetop", S(J));
    runMethod = java_lang_Thread_class->lookupInstMethod(S(run), S(___V));

    auto mainThread = new Thread(pthread_self());

    java_lang_Thread_class->clinit();

    Class *jltgClass = java_lang_ThreadGroup_class;
    sysThreadGroup = Object::newInst(jltgClass);

    // 初始化 system_thread_group
    // java/lang/ThreadGroup 的无参数构造函数主要用来：
    // Creates an empty Thread group that is not in any Thread group.
    // This method is used to create the system Thread group.
    jltgClass->clinit();
    execJavaFunc(jltgClass->getConstructor(S(___V)), sysThreadGroup);

    mainThread->setThreadGroupAndName(sysThreadGroup, MAIN_THREAD_NAME);
    return mainThread;
}

Thread *createVMThread(void *(*start)(void *))
{
    assert(start != nullptr);

    pthread_t pid;
    Thread *vmThread = nullptr;
    void *args[] = { &vmThread, (void *) start };
    int ret = pthread_create(&pid, nullptr,
                             [](void *args) {
                                 auto a = (void **)args;

                                 *(Thread **) a[0] = new Thread();
                                 return ((void *(*)(void *))a[1])(nullptr);
                             }, args);
    if (ret != 0) {
        raiseException(INTERNAL_ERROR, "create Thread failed");
    }

    while (vmThread == nullptr); // 等待子线程设置 vmThread 的值。
    vmThread->pid = pid;
    return vmThread;
}

Thread *createCustomerThread(Object *jThread)
{
    assert(jThread != nullptr);

    pthread_t pid;
    Thread *customer = nullptr;
    void *args[] = { &customer, jThread };
    int ret = pthread_create(&pid, nullptr,
                                [](void *args) {
                                    auto a = (void **)args;

                                    *(Thread **) a[0] = new Thread((jref) a[1]);
                                    return (void *) execJavaFunc(runMethod, (jref) a[1]);
                                }, args);
    if (ret != 0) {
        raiseException(INTERNAL_ERROR, "create Thread failed");
    }

    while (customer == nullptr); // 等待子线程设置 customer 的值。
    customer->pid = pid;
    return customer;
}

Thread::Thread(pthread_t pid, Object *jThread0, jint priority): pid(pid)
{
    new (this)Thread(jThread0, priority);
}

Thread::Thread(Object *jThread0, jint priority): jThread(jThread0)
{
    assert(MIN_PRIORITY <= priority && priority <= MAX_PRIORITY);

    set_thread_self(this);
    g_all_threads.push_back(this);

    if (jThread == nullptr)
        jThread = Object::newInst(java_lang_Thread_class);

    bind(jThread);
    jThread->setFieldValue(S(priority), S(I), (slot_t) priority);
//    if (vmEnv.sysThreadGroup != nullptr)   todo
//        setThreadGroupAndName(vmEnv.sysThreadGroup, nullptr);
}

void Thread::bind(Object *jThread0)
{
    assert(jThread0 != nullptr);
    jThread = jThread0;
    jThread->setFieldValue(eetopField, (slot_t) this);
}

Thread *Thread::from(Object *jThread0)
{
    assert(jThread0 != nullptr);
    return jThread0->getInstFieldValue<Thread *>(eetopField);
}

void Thread::setThreadGroupAndName(Object *threadGroup, const char *threadName)
{
    assert(threadGroup != nullptr);

    if (threadName == nullptr) {
        // todo
        threadName = "unknown";
    }

    // 调用 java/lang/Thread 的构造函数
    Method *constructor = jThread->clazz->getConstructor("(Ljava/lang/ThreadGroup;Ljava/lang/String;)V");
    execJavaFunc(constructor, { (slot_t) jThread,
                                (slot_t) threadGroup,
                                (slot_t) StringObject::newInst(threadName) });
}

bool Thread::isAlive() {
    assert(jThread != nullptr);
    auto status = jThread->getInstFieldValue<jint>("threadStatus", "I");
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
