/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"

// static native boolean isIPv6Supported();
static jbool isIPv6Supported()
{
    // todo
    jvm_abort("not implement");
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    { "isIPv6Supported", "()Z", (void *) isIPv6Supported },
};

void java_net_InetAddressImplFactory_registerNatives()
{
    registerNatives("java/net/InetAddressImplFactory", methods, ARRAY_LENGTH(methods));
}