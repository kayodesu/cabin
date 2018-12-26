/*
 * Author: Jia Yang
 */

#include <pthread.h>
#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/object.h"
#include "../../../interpreter/interpreter.h"

/*
 * Returns a reference to the currently executing thread object.
 *
 * public static native Thread currentThread();
 */
static void currentThread(struct frame *frame)
{
    // push a object of java/lang/Thread of current thread
    frame_stack_pushr(frame, jthread_get_jl_thread_obj(frame->thread));
}

// public static native void yield();
static void yield(struct frame *frame)
{
    jvm_abort(""); // todo
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(struct frame *frame)
{
    jvm_abort("error\n");
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
static void interrupt0(struct frame *frame)
{
    jvm_abort("error\n");
}

// private native boolean isInterrupted(boolean ClearInterrupted);
static void isInterrupted(struct frame *frame)
{
    jvm_abort("error\n");
//    frame->operandStack.push(0);
}

/**
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 *
 * @return  <code>true</code> if this thread is alive;
 *          <code>false</code> otherwise.
 */
// public final native boolean isAlive();
static void isAlive(struct frame *frame)
{
    // todo
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    frame->operandStack.push(0);  // todo wtf?
//    struct object *this_obj = slot_getr(frame->local_vars);
    frame_stack_pushi(frame, 0); // todo 为什么要设置成0，设置成1就状态错误
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
static void setPriority0(struct frame *frame)
{
    // todo
//    jref this = slot_getr(frame->local_vars);
//    jint new_priority = slot_geti(frame->local_vars + 1);
//
//    struct slot priority = islot(new_priority);
//    set_instance_field_value_by_nt(this, "priority", "I", &priority);
}

// private native void start0();
static void start0(struct frame *frame)
{
    // todo
    jref this = frame_locals_getr(frame, 0);

#if (JVM_DEBUG)
    const char *name = jstrobj_value(slot_getr(get_instance_field_value_by_nt(this, "name", "Ljava/lang/String;")));
    printvm("start thread: %s\n", name);
#endif

    struct thread *new_thread = thread_create(frame->m.method->jclass->loader, this);

    // create a stack frame to hold run method
    struct method *run = jclass_lookup_instance_method(this->jclass, "run", "()V");
//    struct frame *new_frame = frmhub_get(&new_thread->fh, run);//frame_create(new_thread, run);
    struct slot arg = rslot(this);

//    frame_locals_set(new_frame, 0, &arg);
//    jthread_push_frame(new_thread, new_frame);

    jthread_invoke_method(new_thread, run, &arg);

//    pthread_t pid;
//    int ret = pthread_create(&pid, NULL, interpret, new_thread);
//    if (ret != 0) {
//        vm_internal_error("create thread failed");
//    }

    /*
     * 	vars := frame.LocalVars()
	this := vars.GetThis()

	newThread := rtda.NewThread(this)
	runMethod := this.Class().GetInstanceMethod("run", "()V")
	newFrame := newThread.NewFrame(runMethod)
	newFrame.LocalVars().SetRef(0, this)
	newThread.PushFrame(newFrame)

	this.LockState()
	this.SetExtra(newThread)
	this.UnlockState()

	go interpreter.Loop(newThread)
     */
}

// @Deprecated
// public native int countStackFrames();
static void countStackFrames(struct frame *frame)
{
    jvm_abort(""); // todo
}

// public static native boolean holdsLock(Object obj);
static void holdsLock(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native static StackTraceElement[][] dumpThreads(Thread[] threads);
static void dumpThreads(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native static Thread[] getThreads();
static void getThreads(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native void stop0(Object o);
static void stop0(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native void suspend0();
static void suspend0(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native void resume0();
static void resume0(struct frame *frame)
{
    jvm_abort(""); // todo
}

// private native void setNativeName(String name);
static void setNativeName(struct frame *frame)
{
    jvm_abort(""); // todo
}

void java_lang_Thread_registerNatives()
{
    register_native_method("java/lang/Thread~registerNatives~()V", registerNatives);
    register_native_method("java/lang/Thread~currentThread~()Ljava/lang/Thread;", currentThread);
    register_native_method("java/lang/Thread~yield~()V", yield);
    register_native_method("java/lang/Thread~sleep~(J)V", sleep);
    register_native_method("java/lang/Thread~interrupt0~()V", interrupt0);
    register_native_method("java/lang/Thread~isInterrupted~(Z)Z", isInterrupted);
    register_native_method("java/lang/Thread~isAlive~()Z", isAlive);
    register_native_method("java/lang/Thread~start0~()V", start0);
    register_native_method("java/lang/Thread~countStackFrames~()I", countStackFrames);
    register_native_method("java/lang/Thread~holdsLock~(Ljava/lang/Object;)Z", holdsLock);
    register_native_method("java/lang/Thread~dumpThreads~([Ljava/lang/Thread;)[[Ljava/lang/StackTraceElement;", dumpThreads);
    register_native_method("java/lang/Thread~getThreads~()[Ljava/lang/Thread;", getThreads);
    register_native_method("java/lang/Thread~setPriority0~(I)V", setPriority0);
    register_native_method("java/lang/Thread~stop0~(Ljava/lang/Object;)V", stop0);
    register_native_method("java/lang/Thread~suspend0~()V", suspend0);
    register_native_method("java/lang/Thread~resume0~()V", resume0);
    register_native_method("java/lang/Thread~setNativeName~(Ljava/lang/String;)V", setNativeName);
}
