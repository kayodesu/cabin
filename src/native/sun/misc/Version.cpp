#include "../../jni_inner.h"
#include "../../../kayo.h"
#include "../../../runtime/frame.h"

/*
 * Author: kayo
 */

// public static native String getJdkSpecialVersion()
static void getJdkSpecialVersion(Frame *frame)
{
    jvm_abort("getJdkSpecialVersion"); // todo
}

// public static native String getJvmSpecialVersion()
static void getJvmSpecialVersion(Frame *frame)
{
    jvm_abort("getJvmSpecialVersion"); // todo
}

static void getJdkVersionInfo(Frame *frame)
{
    jvm_abort("getJdkVersionInfo"); // todo
}

static void getJvmVersionInfo(Frame *frame)
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
