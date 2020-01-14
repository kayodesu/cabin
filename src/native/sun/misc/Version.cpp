#include "../../registry.h"
#include "../../../kayo.h"

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

void sun_misc_Version_registerNatives()
{
    registerNative("sun/misc/Version", "getJdkSpecialVersion", "()Ljava/lang/String;", getJdkSpecialVersion);
    registerNative("sun/misc/Version", "getJvmSpecialVersion", "()Ljava/lang/String;", getJvmSpecialVersion);
    registerNative("sun/misc/Version", "getJdkVersionInfo", "()V", getJdkVersionInfo);
    registerNative("sun/misc/Version", "getJvmVersionInfo", "()Z", getJvmVersionInfo);
}
