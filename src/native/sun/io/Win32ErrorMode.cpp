#include "../../jni_internal.h"

// static
static jlong setErrorMode(jlong l)
{
    return 0; // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "setErrorMode", "(J)J", TA(setErrorMode) },
};

void sun_io_Win32ErrorMode_registerNatives()
{
    registerNatives("sun/io/Win32ErrorMode", methods, ARRAY_LENGTH(methods));
}
