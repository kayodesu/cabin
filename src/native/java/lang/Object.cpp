#include "../../../symbol.h"
#include "../../jni_internal.h"
#include "../../../metadata/class.h"
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
        Thread::signalException(S(java_lang_CloneNotSupportedException));
        return jnull;
    }
    return _this->clone();
}

// public final native Class<?> getClass();
static jobject getClass(jobject _this)
{
    return _this->clazz->java_mirror;
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
        { "hashCode", "()I", TA(hashCode) },
        { "getClass", __CLS, TA(getClass) },
        { "clone", __OBJ, TA(clone) },
        { "notifyAll", "()V", TA(notifyAll) },
        { "notify", "()V", TA(notify) },
        { "wait", "(J)V", TA(wait) },
};

void java_lang_Object_registerNatives()
{
    registerNatives("java/lang/Object", methods, ARRAY_LENGTH(methods));
}
