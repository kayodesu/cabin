#include "../../jni_internal.h"

// private static native boolean isReusePortAvailable0();
static jbool isReusePortAvailable0()
{
    // todo
    JVM_PANIC("not implement");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "isReusePortAvailable0", "()Z", TA(isReusePortAvailable0) },
};

void java_net_AbstractPlainSocketImpl_registerNatives()
{
    registerNatives("java/net/AbstractPlainSocketImpl", methods, ARRAY_LENGTH(methods));
}