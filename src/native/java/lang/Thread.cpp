/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../symbol.h"
#include "../../../objects/object.h"
#include "../../../objects/array_object.h"
#include "../../../runtime/vm_thread.h"
#include "../../../runtime/frame.h"
#include "../../../interpreter/interpreter.h"

using namespace std;

/*
 * Returns a reference to the currently executing thread Object.
 *
 * public static native Thread currentThread();
 */
static jobject currentThread()
{
    // return a Object of java/lang/Thread of current thread
    return getCurrentThread()->tobj;
}

// public static native void yield();
static void yield()
{
    std::this_thread::yield();
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(jlong millis)
{
    if (millis <= 0) {
        Thread::signalException(S(java_lang_IllegalArgumentException));
        return;
    }
    if (millis == 0)
        return;

    this_thread::sleep_for(chrono::microseconds(millis));

    // todo 怎么处理 InterruptedException 
}

// inform VM of interrupt
// private native void interrupt0();
static void interrupt0(jobject _this)
{
    Thread *t = Thread::from(_this);
    t->interrupted = jtrue;
}

/*
 * Tests if some Thread has been interrupted.  The interrupted state
 * is reset or not based on the value of ClearInterrupted that is passed.
 * 
 * private native boolean isInterrupted(boolean ClearInterrupted);
 */
static jboolean isInterrupted(jobject _this, jboolean clearInterrupted)
{
    Thread *t = Thread::from(_this);
    jbool b = t->interrupted;
    if (b && clearInterrupted) {
        t->interrupted = jfalse;
    }
    return b; 
}

/*
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 * 
 * public final native boolean isAlive();
 */
static jboolean isAlive(jobject _this)
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
static void setPriority0(jobject _this, jint newPriority)
{
    // todo
//    struct slot priority = islot(new_priority);
//    set_instance_field_value_by_nt(this, "priority", "I", &priority);
}

// private native void start0();
static void start0(jobject _this)
{
    // createCustomerThread(_this);
    static Method *runMethod 
                = loadBootClass(S(java_lang_Thread))->lookupInstMethod(S(run), S(___V));

    static auto _start = [](Object *_this) {
        new Thread(_this);
        return (void *) execJavaFunc(runMethod, {_this});
    };

    std::thread t(_start, _this);
    t.detach();
}

// public native int countStackFrames();
static jint countStackFrames(jobject _this)
{
    return Thread::from(_this)->countStackFrames();
}

// public static native boolean holdsLock(Object obj);
static jboolean holdsLock(jobject obj)
{
    jvm_abort("holdsLock"); // todo
}

// private native static StackTraceElement[][] dumpThreads(Thread[] threads);
static jobjectArray dumpThreads(jobjectArray threads)
{
    size_t len = threads->size();
    Array *result = newArray(loadArrayClass("[[java/lang/StackTraceElement"), len);

    for (size_t i = 0; i < len; i++) {
        auto jThread = threads->get<jobject>(i);
        Thread *thread = Thread::from(jThread);
        Array *arr = thread->dump(-1);
        result->setRef(i, arr);
    }

    return result;
}

// private native static Thread[] getThreads();
static jobjectArray getThreads()
{
    size_t size = g_all_threads.size();
    Array *threads = newArray(loadArrayClass(S(array_java_lang_Thread)), size);

    for (size_t i = 0; i < size; i++) {
        threads->setRef(i, g_all_threads[i]->tobj);
    }

    return threads;
}

// private native void stop0(Object o);
static void stop0(jobject _this, jobject o)
{
    jvm_abort("stop0"); // todo
}

// private native void suspend0();
static void suspend0(jobject _this)
{
    jvm_abort("suspend0"); // todo
}

// private native void resume0();
static void resume0(jobject _this)
{
    jvm_abort("resume0"); // todo
}

// private static native void clearInterruptEvent();
static void clearInterruptEvent()
{
    jvm_abort("clearInterruptEvent"); // todo
}

// private native void setNativeName(String name);
static void setNativeName(jobject _this, jstring name)
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
        { "clearInterruptEvent", "()V", (void *) clearInterruptEvent },
        { "setNativeName", "(Ljava/lang/String;)V", (void *) setNativeName },
};

void java_lang_Thread_registerNatives()
{
    registerNatives(S(java_lang_Thread), methods, ARRAY_LENGTH(methods));
}
