#include "../../jni_internal.h"

// public static native String getJdkSpecialVersion()
static jstring getJdkSpecialVersion()
{
    jvm_abort("getJdkSpecialVersion"); // todo
}

// public static native String getJvmSpecialVersion()
static jstring getJvmSpecialVersion()
{
    jvm_abort("getJvmSpecialVersion"); // todo
}

static void getJdkVersionInfo()
{
    jvm_abort("getJdkVersionInfo"); // todo
}

static jboolean getJvmVersionInfo()
{
    jvm_abort("getJvmVersionInfo"); // todo
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
