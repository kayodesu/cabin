/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../objects/object.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"
#include "../../../objects/class.h"
#include "../../../runtime/frame.h"

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
    frame->pushr(RSLOT(execJavaFunc(m, _this)));
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

#undef ACC
#define ACC "Ljava/security/AccessControlContext;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getStackAccessControlContext", "()" ACC, (void *) getStackAccessControlContext },
        { "getInheritedAccessControlContext", "()" ACC, (void *) getInheritedAccessControlContext },
        { "doPrivileged", "(Ljava/security/PrivilegedAction;)" OBJ, (void *) doPrivileged },
        { "doPrivileged", "(Ljava/security/PrivilegedAction;" ACC ")" OBJ, (void *) doPrivileged1 },
        { "doPrivileged", "(Ljava/security/PrivilegedExceptionAction;)" OBJ, (void *) doPrivileged2 },
        { "doPrivileged", "(Ljava/security/PrivilegedExceptionAction;" ACC ")" OBJ, (void *) doPrivileged3 },
};

void java_security_AccessController_registerNatives()
{
    registerNatives("java/security/AccessController", methods, ARRAY_LENGTH(methods));
}
