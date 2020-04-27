/*
 * Author: Yo Ka
 */

#include "../../../symbol.h"
#include "../../jni_inner.h"
#include "../../../vmdef.h"
#include "../../../objects/class.h"
#include "../../../objects/object.h"
#include "../../../objects/class_loader.h"

// public native int hashCode();
static jint hashCode(jref _this)
{
    return (jint)(intptr_t)_this;
}

// protected native Object clone() throws CloneNotSupportedException;
static jref clone(jref _this)
{
    if (!_this->clazz->isSubclassOf(loadBootClass(S(java_lang_Cloneable)))) {
        throw CloneNotSupportedException();
    }
    return _this->clone();
}

// public final native Class<?> getClass();
static jref getClass(jref _this)
{
    return _this->clazz; // todo 对不对
}

// public final native void notifyAll();
static void notifyAll(jref _this)
{
    // todo
}

// public final native void notify();
static void notify(jref _this)
{
    // todo
}

// public final native void wait(long timeout) throws InterruptedException;
static void wait(jref _this, jlong timeout)
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
