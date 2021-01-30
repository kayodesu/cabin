#include "../../jni_inner.h"
#include "../../../runtime/frame.h"
#include "../../../objects/object.h"

// private native static String getVersion0();
static jstring getVersion0()
{
    // Management Version
    return newString(MANAGEMENT_VERSION);
}

// private native static void initOptionalSupportFields();
static void initOptionalSupportFields()
{
    jvm_abort("zzz"); // todo
}

// public native boolean isThreadContentionMonitoringEnabled();
static jboolean isThreadContentionMonitoringEnabled(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native boolean isThreadCpuTimeEnabled();
static jboolean isThreadCpuTimeEnabled(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native boolean isThreadAllocatedMemoryEnabled();
static jboolean isThreadAllocatedMemoryEnabled(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native long getTotalClassCount();
static jlong getTotalClassCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native long getUnloadedClassCount();
static jlong getUnloadedClassCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native boolean getVerboseClass();
static jbool getVerboseClass(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// Memory Subsystem
// public native boolean getVerboseGC();
static jbool getVerboseGC(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// private native int getProcessId();
static jint getProcessId(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native String[] getVmArguments0();
static jobjectArray getVmArguments0(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native long getStartupTime();
static jlong getStartupTime(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// private native long getUptime0();
static jlong getUptime0(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native int getAvailableProcessors();
static jint getAvailableProcessors(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native long getTotalCompileTime();
static jlong getTotalCompileTime(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// Thread Subsystem
// public native long getTotalThreadCount();
static jlong getTotalThreadCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native int getLiveThreadCount();
static jint getLiveThreadCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native int getPeakThreadCount();
static jint getPeakThreadCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

// public native int getDaemonThreadCount();
static jint getDaemonThreadCount(jobject _this)
{
    jvm_abort("zzz"); // todo
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    {"getVersion0", "()Ljava/lang/String;", (void *)getVersion0},
    {"initOptionalSupportFields", "()V", (void *)initOptionalSupportFields},
    {"isThreadContentionMonitoringEnabled", "()Z", (void *)isThreadContentionMonitoringEnabled},
    {"isThreadCpuTimeEnabled", "()Z", (void *)isThreadCpuTimeEnabled},
    {"isThreadAllocatedMemoryEnabled", "()Z", (void *)isThreadAllocatedMemoryEnabled},
    {"getTotalClassCount", "()J", (void *)getTotalClassCount},
    {"getUnloadedClassCount", "()J", (void *)getUnloadedClassCount},
    {"getVerboseClass", "()Z", (void *)getVerboseClass},
    {"getVerboseGC", "()Z", (void *)getVerboseGC},
    {"getProcessId", "()I", (void *)getProcessId},
    {"getVmArguments0", "()[java/lang/String;", (void *)getVmArguments0},
    {"getStartupTime", "()J", (void *)getStartupTime},
    {"getUptime0", "()J", (void *)getUptime0},
    {"getAvailableProcessors", "()I", (void *)getAvailableProcessors},
    {"getTotalCompileTime", "()J", (void *)getTotalCompileTime},
    {"getTotalThreadCount", "()J", (void *)getTotalThreadCount},
    {"getLiveThreadCount", "()I", (void *)getLiveThreadCount},
    {"getPeakThreadCount", "()I", (void *)getPeakThreadCount},
    {"getDaemonThreadCount", "()I", (void *)getDaemonThreadCount},
};

void sun_management_VMManagementImpl_registerNatives()
{
    registerNatives("sun/management/VMManagementImpl", methods, ARRAY_LENGTH(methods));
}
