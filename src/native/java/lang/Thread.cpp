/*
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/thread/Jthread.h"

// public static native Thread currentThread();
static void currentThread(StackFrame *frame) {
    // push 一个 java/lang/Thread 对象
    frame->operandStack.push(frame->thread->getJavaLangThreadObj());
}

// public static native void sleep(long millis) throws InterruptedException;
static void sleep(StackFrame *frame) {
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
static void interrupt0(StackFrame *frame) {

}

// private native boolean isInterrupted(boolean ClearInterrupted);
static void isInterrupted(StackFrame *frame) {
    frame->operandStack.push(0);
}

/**
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 *
 * @return  <code>true</code> if this thread is alive;
 *          <code>false</code> otherwise.
 */
// public final native boolean isAlive();
static void isAlive(StackFrame *frame) {
    // todo
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0)->getReference());
    frame->operandStack.push(0);  // todo wtf?
}

// private native void setPriority0(int newPriority);
static void setPriority0(StackFrame *frame) {

}

// private native void start0();
static void start0(StackFrame *frame) {

}

void java_lang_Thread_registerNatives() {
    registerNativeMethod("java/lang/Thread", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Thread", "currentThread", "()Ljava/lang/Thread;", currentThread);
    registerNativeMethod("java/lang/Thread", "sleep", "(J)V", sleep);
    registerNativeMethod("java/lang/Thread", "interrupt0", "()V", interrupt0);
    registerNativeMethod("java/lang/Thread", "isInterrupted", "(Z)Z", isInterrupted);
    registerNativeMethod("java/lang/Thread", "isAlive", "()Z", isAlive);
    registerNativeMethod("java/lang/Thread", "setPriority0", "(I)V", setPriority0);
    registerNativeMethod("java/lang/Thread", "start0", "()V", start0);
}
