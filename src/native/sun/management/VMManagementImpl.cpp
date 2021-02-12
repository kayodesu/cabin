#include "../../jni_internal.h"
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
    JVM_PANIC("zzz"); // todo
}

// public native boolean isThreadContentionMonitoringEnabled();
static jboolean isThreadContentionMonitoringEnabled(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native boolean isThreadCpuTimeEnabled();
static jboolean isThreadCpuTimeEnabled(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native boolean isThreadAllocatedMemoryEnabled();
static jboolean isThreadAllocatedMemoryEnabled(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native long getTotalClassCount();
static jlong getTotalClassCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native long getUnloadedClassCount();
static jlong getUnloadedClassCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native boolean getVerboseClass();
static jbool getVerboseClass(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// Memory Subsystem
// public native boolean getVerboseGC();
static jbool getVerboseGC(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// private native int getProcessId();
static jint getProcessId(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native String[] getVmArguments0();
static jobject getVmArguments0(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native long getStartupTime();
static jlong getStartupTime(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// private native long getUptime0();
static jlong getUptime0(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native int getAvailableProcessors();
static jint getAvailableProcessors(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native long getTotalCompileTime();
static jlong getTotalCompileTime(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// Thread Subsystem
// public native long getTotalThreadCount();
static jlong getTotalThreadCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native int getLiveThreadCount();
static jint getLiveThreadCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native int getPeakThreadCount();
static jint getPeakThreadCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

// public native int getDaemonThreadCount();
static jint getDaemonThreadCount(jobject _this)
{
    JVM_PANIC("zzz"); // todo
}

static JNINativeMethod methods[] = {
    JNINativeMethod_registerNatives,
    {"getVersion0", "()Ljava/lang/String;", TA(getVersion0) },
    {"initOptionalSupportFields", "()V", TA(initOptionalSupportFields) },
    {"isThreadContentionMonitoringEnabled", "()Z", TA(isThreadContentionMonitoringEnabled) },
    {"isThreadCpuTimeEnabled", "()Z", TA(isThreadCpuTimeEnabled) },
    {"isThreadAllocatedMemoryEnabled", "()Z", TA(isThreadAllocatedMemoryEnabled) },
    {"getTotalClassCount", "()J", TA(getTotalClassCount) },
    {"getUnloadedClassCount", "()J", TA(getUnloadedClassCount) },
    {"getVerboseClass", "()Z", TA(getVerboseClass) },
    {"getVerboseGC", "()Z", TA(getVerboseGC) },
    {"getProcessId", "()I", TA(getProcessId) },
    {"getVmArguments0", "()[java/lang/String;", TA(getVmArguments0) },
    {"getStartupTime", "()J", TA(getStartupTime) },
    {"getUptime0", "()J", TA(getUptime0) },
    {"getAvailableProcessors", "()I", TA(getAvailableProcessors) },
    {"getTotalCompileTime", "()J", TA(getTotalCompileTime) },
    {"getTotalThreadCount", "()J", TA(getTotalThreadCount) },
    {"getLiveThreadCount", "()I", TA(getLiveThreadCount) },
    {"getPeakThreadCount", "()I", TA(getPeakThreadCount) },
    {"getDaemonThreadCount", "()I", TA(getDaemonThreadCount) },
};

void sun_management_VMManagementImpl_registerNatives()
{
    registerNatives("sun/management/VMManagementImpl", methods, ARRAY_LENGTH(methods));
}
