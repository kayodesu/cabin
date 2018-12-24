/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action);
static void doPrivileged(struct frame *frame)
{
    // todo 这个函数干什么用的。。。。
    struct jobject *this = frame_locals_getr(frame, 0);

    /*
     * run 函数返回 T类型 的对象
     *
     * public interface PrivilegedAction<T> {
     *     T run();
     * }
     */
    struct jmethod *m = jclass_get_method(this->jclass, "run", "()Ljava/lang/Object;"); // todo getInstanceMethod
    struct slot args[] = { rslot(this) };
    jthread_invoke_method(frame->thread, m, args);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action, AccessControlContext context);
static void doPrivileged1(struct frame *frame)
{
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action) throws PrivilegedActionException;
static void doPrivileged2(struct frame *frame)
{
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action, AccessControlContext context)
//      throws PrivilegedActionException;
static void doPrivileged3(struct frame *frame)
{
    // todo
    doPrivileged(frame);
}

// private static native AccessControlContext getStackAccessControlContext();
static void getStackAccessControlContext(struct frame *frame)
{
    // todo
    frame_stack_pushr(frame, NULL);
}

void java_security_AccessController_registerNatives()
{
    register_native_method("java/security/AccessController~registerNatives~()V", registerNatives);
    register_native_method("java/security/AccessController~"
                                   "getStackAccessControlContext~"
                                   "()Ljava/security/AccessControlContext;",
                           getStackAccessControlContext);

    register_native_method("java/security/AccessController~doPrivileged~"
                                   "(Ljava/security/PrivilegedAction;)Ljava/lang/Object;",
                         doPrivileged);

    register_native_method("java/security/AccessController~doPrivileged~"
             "(Ljava/security/PrivilegedAction;Ljava/security/AccessControlContext;)Ljava/lang/Object;",
                         doPrivileged1);

    register_native_method("java/security/AccessController~doPrivileged~"
                                   "(Ljava/security/PrivilegedExceptionAction;)Ljava/lang/Object;",
                         doPrivileged2);

    register_native_method("java/security/AccessController~doPrivileged~"
             "(Ljava/security/PrivilegedExceptionAction;Ljava/security/AccessControlContext;)Ljava/lang/Object;",
                           doPrivileged3);
}
