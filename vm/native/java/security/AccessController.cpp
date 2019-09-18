/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../rtda/heap/Object.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"
#include "../../../rtda/ma/Class.h"
#include "../../../rtda/thread/Frame.h"

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action);
static void doPrivileged(Frame *frame)
{
    // todo 这个函数干什么用的。。。。
    jref _this = frame->getLocalAsRef(0);

    /*
     * run 函数返回 T类型 的对象
     *
     * public interface PrivilegedAction<T> {
     *     T run();
     * }
     */
    Method *m = _this->clazz->getDeclaredMethod(S(run), S(___java_lang_Object));
    slot_t *s = execJavaFunc(m, _this);
    frame->pushr(RSLOT(s));
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action, AccessControlContext context);
static void doPrivileged1(Frame *frame)
{
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action) throws PrivilegedActionException;
static void doPrivileged2(Frame *frame)
{
    // todo
    doPrivileged(frame);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action, AccessControlContext context)
//      throws PrivilegedActionException;
static void doPrivileged3(Frame *frame)
{
    // todo
    doPrivileged(frame);
}

// private static native AccessControlContext getStackAccessControlContext();
static void getStackAccessControlContext(Frame *frame)
{
    // todo
    frame->pushr(nullptr);
}

// static native AccessControlContext getInheritedAccessControlContext();
static void getInheritedAccessControlContext(Frame *frame)
{
    // todo
    jvm_abort("getInheritedAccessControlContext");
}

void java_security_AccessController_registerNatives()
{
#undef C
#define C "java/security/AccessController",
    register_native_method(C"getStackAccessControlContext", "()Ljava/security/AccessControlContext;",
                           getStackAccessControlContext);

    register_native_method(C"getInheritedAccessControlContext", "()Ljava/security/AccessControlContext;",
                           getInheritedAccessControlContext);

    register_native_method(C"doPrivileged", "(Ljava/security/PrivilegedAction;)" LOBJ, doPrivileged);

    register_native_method(C"doPrivileged",
             "(Ljava/security/PrivilegedAction;Ljava/security/AccessControlContext;)" LOBJ,
                         doPrivileged1);

    register_native_method(C"doPrivileged", "(Ljava/security/PrivilegedExceptionAction;)" LOBJ,
                         doPrivileged2);

    register_native_method(C"doPrivileged",
             "(Ljava/security/PrivilegedExceptionAction;Ljava/security/AccessControlContext;)" LOBJ,
                           doPrivileged3);
}
