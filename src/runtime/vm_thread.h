#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <vector>
#include <thread>
#include "../config.h"
#include "../cabin.h"
#include "../util/encoding.h"

class Object;
class ClassLoader;
class Frame;
class Method;
class Array;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */


/* Thread states */

#define JVMTI_THREAD_STATE_ALIVE                     0x001
#define JVMTI_THREAD_STATE_TERMINATED                0x002
#define JVMTI_THREAD_STATE_RUNNABLE                  0x004
#define JVMTI_THREAD_STATE_WAITING_INDEFINITELY      0x010
#define JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT      0x020
#define JVMTI_THREAD_STATE_SLEEPING                  0x040
#define JVMTI_THREAD_STATE_WAITING                   0x080
#define JVMTI_THREAD_STATE_IN_OBJECT_WAIT            0x100
#define JVMTI_THREAD_STATE_PARKED                    0x200
#define JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER  0x400

#define CREATING           0x0
#define RUNNING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_RUNNABLE)
#define WAITING            (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_INDEFINITELY)
#define TIMED_WAITING      (JVMTI_THREAD_STATE_ALIVE \
                           |JVMTI_THREAD_STATE_WAITING \
                           |JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT)
#define OBJECT_WAIT        (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|WAITING)
#define OBJECT_TIMED_WAIT  (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|TIMED_WAITING)
#define SLEEPING           (JVMTI_THREAD_STATE_SLEEPING|TIMED_WAITING)
#define PARKED             (JVMTI_THREAD_STATE_PARKED|WAITING)
#define TIMED_PARKED       (JVMTI_THREAD_STATE_PARKED|TIMED_WAITING)
#define BLOCKED            JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER
#define TERMINATED         JVMTI_THREAD_STATE_TERMINATED

/* thread priorities */

#define THREAD_MIN_PRIORITY   1
#define THREAD_NORM_PRIORITY  5
#define THREAD_MAX_PRIORITY   10

///* Suspend states */
//
//#define SUSP_NONE      0
//#define SUSP_BLOCKING  1
//#define SUSP_CRITICAL  2
//#define SUSP_SUSPENDED 3
//
///* Park states */
//
//#define PARK_BLOCKED   0
//#define PARK_RUNNING   1
//#define PARK_PERMIT    2

class Monitor;

class Thread {
    /*
     * VM stack 中的 Frame 布局：
     * ------------------------------------------------------------------
     * |lvars|Frame|ostack|, |lvars|Frame|ostack|, |lvars|Frame|ostack| ...
     * ------------------------------------------------------------------
     */
    u1 vm_stack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    Frame *top_frame = nullptr;

    friend Thread *initMainThread();
    friend void createVMThread(void *(*start)(void *), const utf8_t *thread_name);

    friend Object *exceptionOccured();
    friend void signalException(const char *excep_name, const char *message);
    friend void setException(Object *exp);
    friend void clearException();
    friend void printStackTrace();

public:
    Object *tobj = nullptr; // 所关联的 Object of java.lang.Thread
    std::thread::id tid;    // 所关联的 local thread 对应的id

    explicit Thread(Object *jThread = nullptr, jint priority = THREAD_NORM_PRIORITY);

    jbool interrupted = jfalse;

    void setThreadGroupAndName(Object *threadGroup, const char *threadName);

    static Thread *from(Object *jThread0);
    static Thread *from(jlong threadId);

    void setStatus(jint status);
    jint getStatus();

    bool isAlive();

    void clearVMStack()
    {
        top_frame = nullptr;
    }

    Frame *getTopFrame()
    {
        return top_frame;
    }

    Frame *allocFrame(Method *m, bool vm_invoke);
    void popFrame();

    int countStackFrames();
    std::vector<Frame *> getStackFrames();

    /*
     * return a reference of java/lang/management/ThreadInfo
     * where maxDepth < 0 to request entire stack dump
     */
    jref to_java_lang_management_ThreadInfo(jbool lockedMonitors, jbool lockedSynchronizers, jint maxDepth);

    /*
     * return [Ljava/lang/StackTraceElement;
     * where maxDepth < 0 to request entire stack dump
     */
    Array *dump(int maxDepth);

private:
    jref exception = nullptr;
public:
    static jref signalException(const char *excep_name, const char *message = nullptr);
    static void setException(Object *exp);
    static jref getException();
    static bool checkExceptionOccurred();
    static void clearException();
    static void printStackTrace();

    //friend Monitor;
};

extern Thread *g_main_thread;

Thread *initMainThread();

void createVMThread(void *(*start)(void *), const utf8_t *thread_name);

void createCustomerThread(Object *jThread);

Thread *getCurrentThread();

#endif //JVM_JTHREAD_H
