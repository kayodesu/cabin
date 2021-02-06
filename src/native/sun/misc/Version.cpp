#include "../../jni_internal.h"

// public static native String getJdkSpecialVersion()
static jstring getJdkSpecialVersion()
{
    JVM_PANIC("getJdkSpecialVersion"); // todo
}

// public static native String getJvmSpecialVersion()
static jstring getJvmSpecialVersion()
{
    JVM_PANIC("getJvmSpecialVersion"); // todo
}

static void getJdkVersionInfo()
{
    JVM_PANIC("getJdkVersionInfo"); // todo
}

static jboolean getJvmVersionInfo()
{
    JVM_PANIC("getJvmVersionInfo"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getJdkSpecialVersion", __STR, (void *) getJdkSpecialVersion },
        { "getJvmSpecialVersion", __STR, (void *) getJvmSpecialVersion },
        { "getJdkVersionInfo", "()V", (void *) getJdkVersionInfo },
        { "getJvmVersionInfo", "()Z", (void *) getJvmVersionInfo },
};

void sun_misc_Version_registerNatives()
{
    registerNatives("sun/misc/Version", methods, ARRAY_LENGTH(methods));
}
