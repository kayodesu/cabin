/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../jvmstd.h"
#include "../../../sysinfo.h"

// public native int availableProcessors();
static jint availableProcessors(jobject _this)
{
    return (jint) processorNumber();
}

// public native long freeMemory();
static jlong freeMemory(jobject _this)
{
    return freeMemoryCount();
}

// public native long totalMemory();
static jlong totalMemory(jobject _this)
{
    return totalMemoryCount();
}

// public native long maxMemory();
static jlong maxMemory(jobject _this)
{
    return maxMemoryCount();
}

// public native void gc();
static void gc(jobject _this)
{
    jvm_abort("gc");
}

/* Wormhole for calling java.lang.ref.Finalizer.runFinalization */
// private static native void runFinalization0();
static void runFinalization0(jobject _this)
{
    jvm_abort("runFinalization0");
}

// public native void traceInstructions(boolean on)
static void traceInstructions(jobject _this, jboolean on)
{
    jvm_abort("traceInstructions");
}

// public native void traceMethodCalls(boolean on);
static void traceMethodCalls(jobject _this, jboolean on)
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
