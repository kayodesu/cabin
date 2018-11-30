/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"

/*
 * Returns a reference to the currently executing thread object.
 *
 * public static native Thread currentThread();
 */
static void currentThread(struct stack_frame *frame)
{
    // push 一个 java/lang/Thread 对象
    os_pushr(frame->operand_stack, jthread_get_obj(frame->thread));
//    frame->operandStack.push(frame->thread->getJavaLangThreadObj());
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(struct stack_frame *frame)
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
static void interrupt0(struct stack_frame *frame)
{
    jvm_abort("error\n");
}

// private native boolean isInterrupted(boolean ClearInterrupted);
static void isInterrupted(struct stack_frame *frame)
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
static void isAlive(struct stack_frame *frame)
{
    // todo
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    frame->operandStack.push(0);  // todo wtf?
    struct jobject *this_obj = slot_getr(frame->local_vars);
    os_pushi(frame->operand_stack, 1); // todo
}

// private native void setPriority0(int newPriority);
static void setPriority0(struct stack_frame *frame)
{
    // todo
}

// private native void start0();
static void start0(struct stack_frame *frame)
{
    jvm_abort("error\n");
}

void java_lang_Thread_registerNatives()
{
    register_native_method("java/lang/Thread", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/Thread", "currentThread", "()Ljava/lang/Thread;", currentThread);
    register_native_method("java/lang/Thread", "sleep", "(J)V", sleep);
    register_native_method("java/lang/Thread", "interrupt0", "()V", interrupt0);
    register_native_method("java/lang/Thread", "isInterrupted", "(Z)Z", isInterrupted);
    register_native_method("java/lang/Thread", "isAlive", "()Z", isAlive);
    register_native_method("java/lang/Thread", "setPriority0", "(I)V", setPriority0);
    register_native_method("java/lang/Thread", "start0", "()V", start0);
}
