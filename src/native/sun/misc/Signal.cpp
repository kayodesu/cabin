#include "../../jni_inner.h"
#include "../../../runtime/frame.h"

/*
 * Author: Yo Ka
 */

// private static native int findSignal(String string);
static jint findSignal(JNIEnv *env, jclass clazz, jstring string)
{
    return 0; // todo
}

// private static native long handle0(int i, long l);
static jlong handle0(JNIEnv *env, jclass clazz, jint i, jlong l)
{
    return 0; // todo
}

// private static native void raise0(int i);
static void raise0(JNIEnv *env, jclass clazz, jint i)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "findSignal", "(Ljava/lang/String;)I", (void *) findSignal },
        { "handle0", "(IJ)J", (void *) handle0 },
        { "raise0", "(I)V", (void *) raise0 },
};

void sun_misc_Signal_registerNatives()
{
    registerNatives("sun/misc/Signal", methods, ARRAY_LENGTH(methods));
}
