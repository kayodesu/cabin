/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../objects/object.h"
#include "../../../runtime/thread_info.h"
#include "../../../runtime/Frame.h"

/*
 * Returns a reference to the currently executing thread Object.
 *
 * public static native Thread currentThread();
 */
static void currentThread(Frame *frame)
{
    // push a Object of java/lang/Thread of current thread
    frame->pushr(getCurrentThread()->tobj);
}

// public static native void yield();
static void yield(Frame *frame)
{
    jvm_abort("yield"); // todo
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(Frame *frame)
{
    jvm_abort("error\n");

    jlong millis = frame->getLocalAsLong(0);
    if (millis < 0) {
        throw IllegalArgumentException("timeout value is negative");
    }

    // todo

//    vars := frame.LocalVars()
//    millis := vars.GetLong(0)
//
//    thread := frame.Thread()
//    if millis < 0 {
//        thread.ThrowIllegalArgumentException("timeout value is negative")
//        return
//    }
//
//    m := millis * int64(time.Millisecond)
//    d := time.Duration(m)
//    interrupted := thread.Sleep(d)
//
//    if interrupted {
//                thread.ThrowInterruptedException("sleep interrupted")
//        }
}

// private native void interrupt0();
static void interrupt0(Frame *frame)
{
    jvm_abort("error\n");

    jref _this = frame->getLocalAsRef(0);
    // todo
}

// private native boolean isInterrupted(boolean ClearInterrupted);
static void isInterrupted(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);

    frame->pushi(0); // todo
}

/*
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 * 
 * public final native boolean isAlive();
 */
static void isAlive(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);

    frame->pushi(0); // todo 为什么要设置成0，设置成1就状态错误
}

/**
 * Changes the priority of this thread.
 * <p>
 * First the <code>checkAccess</code> method of this thread is called
 * with no arguments. This may result in throwing a
 * <code>SecurityException</code>.
 * <p>
 * Otherwise, the priority of this thread is set to the smaller of
 * the specified <code>newPriority</code> and the maximum permitted
 * priority of the thread's thread group.
 *
 * @param newPriority priority to set this thread to
 * @exception  IllegalArgumentException  If the priority is not in the
 *               range <code>MIN_PRIORITY</code> to
 *               <code>MAX_PRIORITY</code>.
 * @exception  SecurityException  if the current thread cannot modify
 *               this thread.
 * @see        #getPriority
 * @see        #checkAccess()
 * @see        #getThreadGroup()
 * @see        #MAX_PRIORITY
 * @see        #MIN_PRIORITY
 * @see        ThreadGroup#getMaxPriority()
 */
// private native void setPriority0(int newPriority);
static void setPriority0(Frame *frame)
{
    // todo
//    jref this = slot_getr(frame->local_vars);
//    jint new_priority = slot_geti(frame->local_vars + 1);
//
//    struct slot priority = islot(new_priority);
//    set_instance_field_value_by_nt(this, "priority", "I", &priority);
}

// private native void start0();
static void start0(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    createCustomerThread(_this);
}

// public native int countStackFrames();
static void countStackFrames(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    frame->pushi(Thread::from(_this)->countStackFrames());
}

// public static native boolean holdsLock(Object obj);
static void holdsLock(Frame *frame)
{
    jvm_abort("holdsLock"); // todo
}

// private native static StackTraceElement[][] dumpThreads(Thread[] threads);
static void dumpThreads(Frame *frame)
{
    auto threads = frame->getLocalAsRef<Array>(0);

    size_t len = threads->size();
    Array *result = newArray(loadArrayClass("[[java/lang/StackTraceElement"), len);

    for (size_t i = 0; i < len; i++) {
        auto jThread = threads->get<jref>(i);
        Thread *thread = Thread::from(jThread);
        Array *arr = thread->dump(-1);
        result->set(i, arr);
    }

    frame->pushr(result);
}

// private native static Thread[] getThreads();
static void getThreads(Frame *frame)
{
    size_t size = g_all_threads.size();
    Array *threads = newArray(loadArrayClass(S(array_java_lang_Thread)), size);

    for (size_t i = 0; i < size; i++) {
        threads->set(i, g_all_threads[i]->tobj);
    }

    frame->pushr(threads);
}

// private native void stop0(Object o);
static void stop0(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    jvm_abort("stop0"); // todo
}

// private native void suspend0();
static void suspend0(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    jvm_abort("suspend0"); // todo
}

// private native void resume0();
static void resume0(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    jvm_abort("resume0"); // todo
}

// private native void setNativeName(String name);
static void setNativeName(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    jvm_abort("setNativeName"); // todo
}

void java_lang_Thread_registerNatives()
{
#undef C
#define C S(java_lang_Thread)
    registerNative(C, "currentThread", "()Ljava/lang/Thread;", currentThread);
    registerNative(C, "yield", "()V", yield);
    registerNative(C, "sleep", "(J)V", sleep);
    registerNative(C, "interrupt0", "()V", interrupt0);
    registerNative(C, "isInterrupted", "(Z)Z", isInterrupted);
    registerNative(C, "isAlive", "()Z", isAlive);
    registerNative(C, "start0", "()V", start0);
    registerNative(C, "countStackFrames", "()I", countStackFrames);
    registerNative(C, "holdsLock", "(Ljava/lang/Object;)Z", holdsLock);
    registerNative(C, "dumpThreads", "([Ljava/lang/Thread;)[[Ljava/lang/StackTraceElement;", dumpThreads);
    registerNative(C, "getThreads", "()[Ljava/lang/Thread;", getThreads);
    registerNative(C, "setPriority0", "(I)V", setPriority0);
    registerNative(C, "stop0", "(Ljava/lang/Object;)V", stop0);
    registerNative(C, "suspend0", "()V", suspend0);
    registerNative(C, "resume0", "()V", resume0);
    registerNative(C, "setNativeName", "(Ljava/lang/String;)V", setNativeName);
}
