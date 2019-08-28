/*
 * Author: kayo
 */

#ifndef JVM_JTHREAD_H
#define JVM_JTHREAD_H

#include <pthread.h>
#include "../../config.h"
#include "../../jtypes.h"

class Object;
class ClassLoader;
class Frame;
class Method;

/*
 * jvm中所定义的线程
 *
 * 如果Java虚拟机栈有大小限制，且执行线程所需的栈空间超出了这个限制，
 * 会导致StackOverflowError异常抛出。如果Java虚拟机栈可以动态扩展，
 * 但是内存已经耗尽，会导致OutOfMemoryError异常抛出。
 */


///* Thread states */
//
//#define JVMTI_THREAD_STATE_ALIVE                    0x001
//#define JVMTI_THREAD_STATE_TERMINATED               0x002
//#define JVMTI_THREAD_STATE_RUNNABLE                 0x004
//#define JVMTI_THREAD_STATE_WAITING_INDEFINITELY     0x010
//#define JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT     0x020
//#define JVMTI_THREAD_STATE_SLEEPING                 0x040
//#define JVMTI_THREAD_STATE_WAITING                  0x080
//#define JVMTI_THREAD_STATE_IN_OBJECT_WAIT           0x100
//#define JVMTI_THREAD_STATE_PARKED                   0x200
//#define JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER 0x400
//
//#define CREATING          0x0
//#define RUNNING           (JVMTI_THREAD_STATE_ALIVE \
//                          |JVMTI_THREAD_STATE_RUNNABLE)
//#define WAITING           (JVMTI_THREAD_STATE_ALIVE \
//                          |JVMTI_THREAD_STATE_WAITING \
//                          |JVMTI_THREAD_STATE_WAITING_INDEFINITELY)
//#define TIMED_WAITING     (JVMTI_THREAD_STATE_ALIVE \
//                          |JVMTI_THREAD_STATE_WAITING \
//                          |JVMTI_THREAD_STATE_WAITING_WITH_TIMEOUT)
//#define OBJECT_WAIT       (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|WAITING)
//#define OBJECT_TIMED_WAIT (JVMTI_THREAD_STATE_IN_OBJECT_WAIT|TIMED_WAITING)
//#define SLEEPING          (JVMTI_THREAD_STATE_SLEEPING|TIMED_WAITING)
//#define PARKED            (JVMTI_THREAD_STATE_PARKED|WAITING)
//#define TIMED_PARKED      (JVMTI_THREAD_STATE_PARKED|TIMED_WAITING)
//#define BLOCKED           JVMTI_THREAD_STATE_BLOCKED_ON_MONITOR_ENTER
//#define TERMINATED        JVMTI_THREAD_STATE_TERMINATED

/* thread priorities */

#define MIN_PRIORITY   1
#define NORM_PRIORITY  5
#define MAX_PRIORITY   10

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

class Monitor {
    /*
    pthread_mutex_t lock;
    pthread_cond_t cv;
    Thread *owner;
    int count;
    int waiting;
    int notifying;
    int interrupting;
    int entering;
    struct monitor *next;
    char in_use;
     */
};

class Thread {
    void bind(Object *jThread0);
    static Thread *from(Object *jThread0);
public:
    Object *jThread = nullptr; // 所关联的 Object of java.lang.Thread
    pthread_t pid;  // 所关联的 POSIX 线程对应的id

    u1 vmStack[VM_STACK_SIZE]; // 虚拟机栈，一个线程只有一个虚拟机栈
    Frame *topFrame = nullptr;

    explicit Thread(pthread_t pid, Object *jThread = nullptr, jint priority = NORM_PRIORITY);
    explicit Thread(Object *jThread = nullptr, jint priority = NORM_PRIORITY);

    void setThreadGroupAndName(Object *threadGroup, const char *threadName);

    bool isAlive();
};

Thread *initMainThread();
Thread *createVMThread(void *(*start)(void *));
Thread *createCustomerThread(Object *jThread);

Thread *thread_self();

Frame *allocFrame(Method *m, bool vm_invoke);
void popFrame();

int vm_stack_depth();

void thread_handle_uncaught_exception(Object *exception);

void thread_throw_null_pointer_exception();
void thread_throw_negative_array_size_exception(int array_size);
void thread_throw_array_index_out_of_bounds_exception(int index);
void thread_throw_class_cast_exception(const char *from_class_name, const char *to_class_name);

#endif //JVM_JTHREAD_H
