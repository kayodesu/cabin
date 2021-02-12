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
        { "getJdkSpecialVersion", __STR, TA(getJdkSpecialVersion) },
        { "getJvmSpecialVersion", __STR, TA(getJvmSpecialVersion) },
        { "getJdkVersionInfo", "()V", TA(getJdkVersionInfo) },
        { "getJvmVersionInfo", "()Z", TA(getJvmVersionInfo) },
};

void sun_misc_Version_registerNatives()
{
    registerNatives("sun/misc/Version", methods, ARRAY_LENGTH(methods));
}
