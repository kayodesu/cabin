#include "../../jni_internal.h"

// private static native boolean isReusePortAvailable0();
static jbool isReusePortAvailable0()
{
    // todo
    JVM_PANIC("not implement");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "isReusePortAvailable0", "()Z", (void *) isReusePortAvailable0 },
};

void java_net_AbstractPlainDatagramSocketImpl_registerNatives()
{
    registerNatives("java/net/AbstractPlainDatagramSocketImpl", methods, ARRAY_LENGTH(methods));
}