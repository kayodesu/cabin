/*
 * Author: kayo
 */

#include <windows.h> // todo
#include "../../registry.h"
#include "../../../runtime/frame.h"

// public native int availableProcessors();
static void availableProcessors(Frame *frame)
{
    // todo 这是windows 方式，不利于平台移植
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    frame->pushi((jint) sysInfo.dwNumberOfProcessors);
}

// public native long freeMemory();
static void freeMemory(Frame *frame)
{
    jvm_abort("freeMemory");
}

// public native long totalMemory();
static void totalMemory(Frame *frame)
{
    jvm_abort("totalMemory");
}

// public native long maxMemory();
static void maxMemory(Frame *frame)
{
    jvm_abort("maxMemory");
}

// public native void gc();
static void gc(Frame *frame)
{
    jvm_abort("gc");
}

/* Wormhole for calling java.lang.ref.Finalizer.runFinalization */
// private static native void runFinalization0();
static void runFinalization0(Frame *frame)
{
    jvm_abort("runFinalization0");
}

// public native void traceInstructions(boolean on)
static void traceInstructions(Frame *frame)
{
    jvm_abort("traceInstructions");
}

// public native void traceMethodCalls(boolean on);
static void traceMethodCalls(Frame *frame)
{
    jvm_abort("traceMethodCalls");
}

void java_lang_Runtime_registerNatives()
{
    registerNative("java/lang/Runtime", "availableProcessors", "()I", availableProcessors);
    registerNative("java/lang/Runtime", "freeMemory", "()J", freeMemory);
    registerNative("java/lang/Runtime", "totalMemory", "()J", totalMemory);
    registerNative("java/lang/Runtime", "maxMemory", "()J", maxMemory);
    registerNative("java/lang/Runtime", "gc", "()V", gc);
    registerNative("java/lang/Runtime", "runFinalization0", "()V", runFinalization0);
    registerNative("java/lang/Runtime", "traceInstructions", "(Z)V", traceInstructions);
    registerNative("java/lang/Runtime", "traceMethodCalls", "(Z)V", traceMethodCalls);
}
