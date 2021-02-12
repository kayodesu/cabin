#include "../../jni_internal.h"

// Perform class load-time initializations.
// private static native void init();
static void init()
{
    // todo
    JVM_PANIC("not implement");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "init", "()V", TA(init) },
};

void java_net_InetAddress_registerNatives()
{
    registerNatives("java/net/InetAddress", methods, ARRAY_LENGTH(methods));
}