#include "../../jni_inner.h"

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
        { "getJdkSpecialVersion", "()Ljava/lang/String;", (void *) getJdkSpecialVersion },
        { "getJvmSpecialVersion", "()Ljava/lang/String;", (void *) getJvmSpecialVersion },
        { "getJdkVersionInfo", "()V", (void *) getJdkVersionInfo },
        { "getJvmVersionInfo", "()Z", (void *) getJvmVersionInfo },
};

void sun_misc_Version_registerNatives()
{
    registerNatives("sun/misc/Version", methods, ARRAY_LENGTH(methods));
}
