#include "../../jni_internal.h"
#include "../../../symbol.h"
#include "../../../slot.h"
#include "../../../metadata/method.h"
#include "../../../metadata/class.h"
#include "../../../interpreter/interpreter.h"

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action);
static jobject doPrivileged(jobject action)
{
    // todo 这个函数干什么用的。。。。

    /*
     * run 函数返回 T类型 的对象
     *
     * public interface PrivilegedAction<T> {
     *     T run();
     * }
     */
    Method *m = action->clazz->getDeclaredMethod(S(run), S(___java_lang_Object));

    return slot::getRef(execJavaFunc(m, {action}));
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedAction<T> action, AccessControlContext context);
static jobject doPrivileged1(jobject action, jobject context)
{
    // todo
    return doPrivileged(action);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action) throws PrivilegedActionException;
static jobject doPrivileged2(jobject action)
{
    // todo
    return doPrivileged(action);
}

// @CallerSensitive
// public static native <T> T doPrivileged(PrivilegedExceptionAction<T> action, AccessControlContext context)
//      throws PrivilegedActionException;
static jobject doPrivileged3(jobject action, jobject context)
{
    // todo
    return doPrivileged(action);
}

// private static native AccessControlContext getStackAccessControlContext();
static jobject getStackAccessControlContext()
{
    // todo
    return nullptr;
}

// static native AccessControlContext getInheritedAccessControlContext();
static jobject getInheritedAccessControlContext()
{
    // todo
    JVM_PANIC("getInheritedAccessControlContext");
}

#undef ACC
#define ACC "Ljava/security/AccessControlContext;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getStackAccessControlContext", "()" ACC, TA(getStackAccessControlContext) },
        { "getInheritedAccessControlContext", "()" ACC, TA(getInheritedAccessControlContext) },
        { "doPrivileged", "(Ljava/security/PrivilegedAction;)" OBJ, TA(doPrivileged) },
        { "doPrivileged", "(Ljava/security/PrivilegedAction;" ACC ")" OBJ, TA(doPrivileged1) },
        { "doPrivileged", "(Ljava/security/PrivilegedExceptionAction;)" OBJ, TA(doPrivileged2) },
        { "doPrivileged", "(Ljava/security/PrivilegedExceptionAction;" ACC ")" OBJ, TA(doPrivileged3) },
};

#undef ACC

void java_security_AccessController_registerNatives()
{
    registerNatives("java/security/AccessController", methods, ARRAY_LENGTH(methods));
}
