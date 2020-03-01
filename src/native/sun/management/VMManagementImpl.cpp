#include "../../registry.h"
#include "../../../runtime/Frame.h"
#include "../../../objects/object.h"

/*
 * Author: kayo
 */

// private native static String getVersion0();
static void getVersion0(Frame *frame)
{
    // Management Version
    frame->pushr(newString(MANAGEMENT_VERSION));
}

// private native static void initOptionalSupportFields();
static void initOptionalSupportFields(Frame *frame)
{
    // todo
}

// public native boolean isThreadContentionMonitoringEnabled();
static void isThreadContentionMonitoringEnabled(Frame *frame)
{
    frame->pushi(1); // todo
}

// public native boolean isThreadCpuTimeEnabled();
static void isThreadCpuTimeEnabled(Frame *frame)
{
    frame->pushi(1); // todo
}

// public native boolean isThreadAllocatedMemoryEnabled();
static void isThreadAllocatedMemoryEnabled(Frame *frame)
{
    frame->pushi(1); // todo
}

// public native long getTotalClassCount();
// public native long getUnloadedClassCount();

// public native boolean getVerboseClass();

// Memory Subsystem
// public native boolean getVerboseGC();

// private native int getProcessId();

// public native String[] getVmArguments0();

// public native long getStartupTime();
// private native long getUptime0();
// public native int getAvailableProcessors();

// public native long getTotalCompileTime();

// Thread Subsystem
// public native long getTotalThreadCount();
// public native int  getLiveThreadCount();
// public native int  getPeakThreadCount();
// public native int  getDaemonThreadCount();

void sun_management_VMManagementImpl_registerNatives()
{
#undef C
#define C "sun/management/VMManagementImpl"
    registerNative(C, "getVersion0", "()Ljava/lang/String;", getVersion0);
    registerNative(C, "initOptionalSupportFields", "()V", initOptionalSupportFields);
    registerNative(C, "isThreadContentionMonitoringEnabled", "()Z", isThreadContentionMonitoringEnabled);
    registerNative(C, "isThreadCpuTimeEnabled", "()Z", isThreadCpuTimeEnabled);
    registerNative(C, "isThreadAllocatedMemoryEnabled", "()Z", isThreadAllocatedMemoryEnabled);
}
