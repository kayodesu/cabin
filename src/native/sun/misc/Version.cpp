#include "../../registry.h"
#include "../../../jvm.h"

/*
 * Author: Jia Yang
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
    register_native_method("sun/misc/Version", "getJdkSpecialVersion", "()Ljava/lang/String;", getJdkSpecialVersion);
    register_native_method("sun/misc/Version", "getJvmSpecialVersion", "()Ljava/lang/String;", getJvmSpecialVersion);
    register_native_method("sun/misc/Version", "getJdkVersionInfo", "()V", getJdkVersionInfo);
    register_native_method("sun/misc/Version", "getJvmVersionInfo", "()Z", getJvmVersionInfo);
}
