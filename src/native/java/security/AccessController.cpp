/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action);
static void doPrivileged(StackFrame *frame) {
    // todo 这个函数干什么用的。。。。
    Jobject *privilegedAction = static_cast<Jobject *>(frame->getLocalVars(0).getReference());

    /*
     * run 函数返回 T类型 的对象
     *
     * public interface PrivilegedAction<T> {
     *     T run();
     * }
     */
    frame->operandStack.push(privilegedAction);  // push 'this' for invoke function 'run'.
    Jmethod *m = privilegedAction->getClass()->getMethod("run", "()Ljava/lang/Object;"); // todo getInstanceMethod
    frame->invokeMethod(m);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action, AccessControlContext context);
static void doPrivileged1(StackFrame *frame) {
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action) throws PrivilegedActionException;
static void doPrivileged2(StackFrame *frame) {
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action, AccessControlContext context)
//      throws PrivilegedActionException;
static void doPrivileged3(StackFrame *frame) {
    // todo
    doPrivileged(frame);
}

// private static native AccessControlContext getStackAccessControlContext();
//
static void getStackAccessControlContext(StackFrame *frame) {
    // todo
    frame->operandStack.push(nullptr);
}

void java_security_AccessController_registerNatives() {
    registerNativeMethod("java/security/AccessController", "registerNatives", "()V", [](StackFrame *){});

    registerNativeMethod("java/security/AccessController", "getStackAccessControlContext",
                         "()Ljava/security/AccessControlContext;", getStackAccessControlContext);

    registerNativeMethod("java/security/AccessController", "doPrivileged",
                         "(Ljava/security/PrivilegedAction;)Ljava/lang/Object;",
                         doPrivileged);

    registerNativeMethod("java/security/AccessController", "doPrivileged",
                         "(Ljava/security/PrivilegedAction;Ljava/security/AccessControlContext;)Ljava/lang/Object;",
                         doPrivileged1);

    registerNativeMethod("java/security/AccessController", "doPrivileged",
                         "(Ljava/security/PrivilegedExceptionAction;)Ljava/lang/Object;",
                         doPrivileged2);

    registerNativeMethod("java/security/AccessController", "doPrivileged",
                         "(Ljava/security/PrivilegedExceptionAction;Ljava/security/AccessControlContext;)Ljava/lang/Object;",
                         doPrivileged3);

}