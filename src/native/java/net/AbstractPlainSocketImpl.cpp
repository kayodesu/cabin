#include "../../jni_internal.h"

// private static native boolean isReusePortAvailable0();
static jbool isReusePortAvailable0()
{
    // todo
    jvm_abort("not implement");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "isReusePortAvailable0", "()Z", (void *) isReusePortAvailable0},
};

void java_net_AbstractPlainSocketImpl_registerNatives()
{
    registerNatives("java/net/AbstractPlainSocketImpl", methods, ARRAY_LENGTH(methods));
}