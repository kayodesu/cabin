#include "../../jni_inner.h"

// Perform class load-time initializations.
// private static native void init();
static void init()
{
    // todo
    jvm_abort("not implement");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "init", "()V", (void *) init },
};

void java_net_InetAddress_registerNatives()
{
    registerNatives("java/net/InetAddress", methods, ARRAY_LENGTH(methods));
}