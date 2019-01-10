/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/object.h"

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action);
static void doPrivileged(struct frame *frame)
{
    // todo 这个函数干什么用的。。。。
    struct object *this = frame_locals_getr(frame, 0);

    /*
     * run 函数返回 T类型 的对象
     *
     * public interface PrivilegedAction<T> {
     *     T run();
     * }
     */
    struct method *m = class_get_declared_nonstatic_method(this->clazz, "run", "()Ljava/lang/Object;");
    struct slot args[] = { rslot(this) };
    thread_invoke_method(frame->thread, m, args);
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

// static native AccessControlContext getInheritedAccessControlContext();
static void getInheritedAccessControlContext(struct frame *frame)
{
    // todo
    jvm_abort("");
}

void java_security_AccessController_registerNatives()
{
#undef C
#define C "java/security/AccessController~"
    register_native_method(C"registerNatives~()V", registerNatives);
    register_native_method(C"getStackAccessControlContext~()Ljava/security/AccessControlContext;",
                           getStackAccessControlContext);

    register_native_method(C"getInheritedAccessControlContext~()Ljava/security/AccessControlContext;",
                           getInheritedAccessControlContext);

    register_native_method(C"doPrivileged~(Ljava/security/PrivilegedAction;)"LOBJ, doPrivileged);

    register_native_method(C"doPrivileged~"
             "(Ljava/security/PrivilegedAction;Ljava/security/AccessControlContext;)"LOBJ,
                         doPrivileged1);

    register_native_method(C"doPrivileged~(Ljava/security/PrivilegedExceptionAction;)"LOBJ,
                         doPrivileged2);

    register_native_method(C"doPrivileged~"
             "(Ljava/security/PrivilegedExceptionAction;Ljava/security/AccessControlContext;)"LOBJ,
                           doPrivileged3);
}
