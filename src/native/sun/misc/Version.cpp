#include "../../jni_inner.h"
#include "../../../kayo.h"
#include "../../../runtime/frame.h"

/*
 * Author: Yo Ka
 */

// public static native String getJdkSpecialVersion()
static jstring getJdkSpecialVersion(JNIEnv *env, jclass clazz)
{
    jvm_abort("getJdkSpecialVersion"); // todo
}

// public static native String getJvmSpecialVersion()
static jstring getJvmSpecialVersion(JNIEnv *env, jclass clazz)
{
    jvm_abort("getJvmSpecialVersion"); // todo
}

static void getJdkVersionInfo(JNIEnv *env, jclass clazz)
{
    jvm_abort("getJdkVersionInfo"); // todo
}

static jboolean getJvmVersionInfo(JNIEnv *env, jclass clazz)
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
