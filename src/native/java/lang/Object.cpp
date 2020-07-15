/*
 * Author: Yo Ka
 */

#include "../../../symbol.h"
#include "../../jni_inner.h"
#include "../../../jvmstd.h"
#include "../../../metadata/class.h"
#include "../../../objects/object.h"
#include "../../../objects/class_loader.h"
#include "../../../runtime/vm_thread.h"

// public native int hashCode();
static jint hashCode(jobject _this)
{
    return (jint)(intptr_t)_this;
}

// protected native Object clone() throws CloneNotSupportedException;
static jobject clone(jobject _this)
{
    if (!_this->clazz->isSubclassOf(loadBootClass(S(java_lang_Cloneable)))) {
        signalException(S(java_lang_CloneNotSupportedException));
        return jnull;
    }
    return _this->clone();
}

// public final native Class<?> getClass();
static jobject getClass(jobject _this)
{
    return _this->clazz->java_mirror; // todo 对不对
}

// public final native void notifyAll();
static void notifyAll(jobject _this)
{
    // todo
}

// public final native void notify();
static void notify(jobject _this)
{
    // todo
}

// public final native void wait(long timeout) throws InterruptedException;
static void wait(jobject _this, jlong timeout)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "hashCode", "()I", (void *) hashCode },
        { "getClass", "()Ljava/lang/Class;", (void *) getClass },
        { "clone", "()Ljava/lang/Object;", (void *) clone },
        { "notifyAll", "()V", (void *) notifyAll },
        { "notify", "()V", (void *) notify },
        { "wait", "(J)V", (void *) wait },
};

void java_lang_Object_registerNatives()
{
    registerNatives("java/lang/Object", methods, ARRAY_LENGTH(methods));
}
