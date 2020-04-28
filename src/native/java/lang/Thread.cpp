/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../objects/object.h"
#include "../../../objects/array_object.h"
#include "../../../runtime/thread_info.h"
#include "../../../runtime/frame.h"

/*
 * Returns a reference to the currently executing thread Object.
 *
 * public static native Thread currentThread();
 */
static jref currentThread()
{
    // return a Object of java/lang/Thread of current thread
    return getCurrentThread()->tobj;
}

// public static native void yield();
static void yield()
{
    jvm_abort("yield"); // todo
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(jlong millis)
{
    jvm_abort("error\n");

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
static void interrupt0(jref _this)
{
    jvm_abort("error\n"); // todo
}

// private native boolean isInterrupted(boolean ClearInterrupted);
static jboolean isInterrupted(jref _this)
{
    return jfalse; // todo
}

/*
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 * 
 * public final native boolean isAlive();
 */
static jboolean isAlive(jref _this)
{
    return jfalse; // todo 为什么要设置成0，设置成1就状态错误
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
static void setPriority0(jref _this, jint newPriority)
{
    // todo
//    struct slot priority = islot(new_priority);
//    set_instance_field_value_by_nt(this, "priority", "I", &priority);
}

// private native void start0();
static void start0(jref _this)
{
    createCustomerThread(_this);
}

// public native int countStackFrames();
static jint countStackFrames(jref _this)
{
    return Thread::from(_this)->countStackFrames();
}

// public static native boolean holdsLock(Object obj);
static jboolean holdsLock(jref obj)
{
    jvm_abort("holdsLock"); // todo
}

// private native static StackTraceElement[][] dumpThreads(Thread[] threads);
static jarrref dumpThreads(jarrref threads)
{
    size_t len = threads->size();
    Array *result = newArray(loadArrayClass("[[java/lang/StackTraceElement"), len);

    for (size_t i = 0; i < len; i++) {
        auto jThread = threads->get<jref>(i);
        Thread *thread = Thread::from(jThread);
        Array *arr = thread->dump(-1);
        result->set(i, arr);
    }

    return result;
}

// private native static Thread[] getThreads();
static jarrref getThreads()
{
    size_t size = g_all_threads.size();
    Array *threads = newArray(loadArrayClass(S(array_java_lang_Thread)), size);

    for (size_t i = 0; i < size; i++) {
        threads->set(i, g_all_threads[i]->tobj);
    }

    return threads;
}

// private native void stop0(Object o);
static void stop0(jref _this, jref o)
{
    jvm_abort("stop0"); // todo
}

// private native void suspend0();
static void suspend0(jref _this)
{
    jvm_abort("suspend0"); // todo
}

// private native void resume0();
static void resume0(jref _this)
{
    jvm_abort("resume0"); // todo
}

// private native void setNativeName(String name);
static void setNativeName(jref _this, jstrref name)
{
    jvm_abort("setNativeName"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "currentThread", "()Ljava/lang/Thread;", (void *) currentThread },
        { "yield", "()V", (void *) yield },
        { "sleep", "(J)V", (void *) sleep },
        { "interrupt0", "()V", (void *) interrupt0 },
        { "isInterrupted", "(Z)Z", (void *) isInterrupted },
        { "isAlive", "()Z", (void *) isAlive },
        { "start0", "()V", (void *) start0 },
        { "countStackFrames", "()I", (void *) countStackFrames },
        { "holdsLock", "(Ljava/lang/Object;)Z", (void *) holdsLock },
        { "dumpThreads", "([Ljava/lang/Thread;)[[Ljava/lang/StackTraceElement;", (void *) dumpThreads },
        { "getThreads", "()[Ljava/lang/Thread;", (void *) getThreads },
        { "setPriority0", "(I)V", (void *) setPriority0 },
        { "stop0", "(Ljava/lang/Object;)V", (void *) stop0 },
        { "suspend0", "()V", (void *) suspend0 },
        { "resume0", "()V", (void *) resume0 },
        { "setNativeName", "(Ljava/lang/String;)V", (void *) setNativeName },
};

void java_lang_Thread_registerNatives()
{
    registerNatives(S(java_lang_Thread), methods, ARRAY_LENGTH(methods));
}
