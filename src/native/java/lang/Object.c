/*
 * Author: Yo Ka
 */

#include "../../../symbol.h"
#include "../../jnidef.h"
#include "../../cli.h"

// public native int hashCode();
static jint hashCode(JNIEnv *env, jobject _this)
{
    return (jint)(intptr_t)_this;
}

// protected native Object clone() throws CloneNotSupportedException;
static jobject clone(JNIEnv *env, jobject _this)
{
    jclass c = (*env)->GetObjectClass(env, _this);
    if (cli.isSubclassOf(c, (*env)->FindClass(env, S(java_lang_Cloneable))) == 0) {
        jclass exception = (*env)->FindClass(env, S(java_lang_CloneNotSupportedException));
        (*env)->ThrowNew(env, exception, NULL);
    }
    return cli.cloneObject(_this);
}

// public final native Class<?> getClass();
static jobject getClass(JNIEnv *env, jobject _this)
{
    return (*env)->GetObjectClass(env, _this); // todo 对不对
}

// public final native void notifyAll();
static void notifyAll(JNIEnv *env, jobject _this)
{
    // todo
}

// public final native void notify();
static void notify(JNIEnv *env, jobject _this)
{
    // todo
}

// public final native void wait(long timeout) throws InterruptedException;
static void wait(JNIEnv *env, jobject _this, jlong timeout)
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
