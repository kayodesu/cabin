#include "../../jni_internal.h"

// static native boolean isIPv6Supported();
static jbool isIPv6Supported()
{
    // todo
    JVM_PANIC("not implement");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "isIPv6Supported", "()Z", TA(isIPv6Supported) },
};

void java_net_InetAddressImplFactory_registerNatives()
{
    registerNatives("java/net/InetAddressImplFactory", methods, ARRAY_LENGTH(methods));
}