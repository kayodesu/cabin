/*
 * Author: Yo Ka
 */

#include <windows.h> // todo
#include "../../jni_inner.h"
#include "../../../vmdef.h"

// public native int availableProcessors();
static jint availableProcessors(jref _this)
{
    // todo 这是windows 方式，不利于平台移植
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return (jint)sysInfo.dwNumberOfProcessors;
}

// public native long freeMemory();
static jlong freeMemory(jref _this)
{
    jvm_abort("freeMemory");
}

// public native long totalMemory();
static jlong totalMemory(jref _this)
{
    jvm_abort("totalMemory");
}

// public native long maxMemory();
static jlong maxMemory(jref _this)
{
    jvm_abort("maxMemory");
}

// public native void gc();
static void gc(jref _this)
{
    jvm_abort("gc");
}

/* Wormhole for calling java.lang.ref.Finalizer.runFinalization */
// private static native void runFinalization0();
static void runFinalization0(jref _this)
{
    jvm_abort("runFinalization0");
}

// public native void traceInstructions(boolean on)
static void traceInstructions(jref _this)
{
    jvm_abort("traceInstructions");
}

// public native void traceMethodCalls(boolean on);
static void traceMethodCalls(jref _this, jboolean on)
{
    jvm_abort("traceMethodCalls");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "availableProcessors", "()I", (void *) availableProcessors },
        { "freeMemory", "()J", (void *) freeMemory },
        { "totalMemory", "()J", (void *) totalMemory },
        { "maxMemory", "()J", (void *) maxMemory },
        { "gc", "()V", (void *) gc },
        { "runFinalization0", "()V", (void *) runFinalization0 },
        { "traceInstructions", "(Z)V", (void *) traceInstructions },
        { "traceMethodCalls", "(Z)V", (void *) traceMethodCalls },
};

void java_lang_Runtime_registerNatives()
{
    registerNatives("java/lang/Runtime", methods, ARRAY_LENGTH(methods));
}
