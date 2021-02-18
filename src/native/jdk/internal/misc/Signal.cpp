#include "../../../jni_internal.h"

// private static native int findSignal(String string);
static jint findSignal(jstring string)
{
    return 0; // todo
}

// private static native long handle0(int i, long l);
static jlong handle0(jint i, jlong l)
{
    return 0; // todo
}

// private static native void raise0(int i);
static void raise0(jint i)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "findSignal", _STR_ "I", TA(findSignal) },
        { "findSignal0", _STR_ "I", TA(findSignal) },
        { "handle0", "(IJ)J", TA(handle0) },
        { "raise0", "(I)V", TA(raise0) },
};

void sun_misc_Signal_registerNatives()
{
    if (IS_GDK9_PLUS) {
        registerNatives("jdk/internal/misc/Signal", methods, ARRAY_LENGTH(methods));
    } else {
        registerNatives("sun/misc/Signal", methods, ARRAY_LENGTH(methods));
    }
}
