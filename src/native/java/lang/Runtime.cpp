#include "../../jni_internal.h"
#include "../../../cabin.h"
#include "../../../config.h"
#include "../../../heap/heap.h"
#include "../../../platform/sysinfo.h"

// public native int availableProcessors();
static jint availableProcessors(jobject _this)
{
    return (jint) processorNumber();
}

/* todo
 * freeMemory()是什么呢，刚才讲到如果在运行java的时候没有添加-Xms参数，那么，在java程序运行的过程的，内存总是慢慢的从操 作系统那里挖的，基本上是用多少挖多少，但是java虚拟机100％的情况下是会稍微多挖一点的，这些挖过来而又没有用上的内存，实际上就是 freeMemory()，所以freeMemory()的值一般情况下都是很小的，但是如果你在运行java程序的时候使用了-Xms，这个时候因为程 序在启动的时候就会无条件的从操作系统中挖-Xms后面定义的内存数，这个时候，挖过来的内存可能大部分没用上，所以这个时候freeMemory()可 能会有些大。
 */
// public native long freeMemory();
static jlong freeMemory(jobject _this)
{
    return g_heap->freeMemory();
}

/* todo
 * totalMemory()这个方法返回的是java虚拟机现在已经从操作系统那里挖过来的内存大小，也就是java虚拟机这个进程当时所占用的所有 内存。如果在运行java的时候没有添加-Xms参数，那么，在java程序运行的过程的，内存总是慢慢的从操作系统那里挖的，基本上是用多少挖多少，直 挖到maxMemory()为止，所以totalMemory()是慢慢增大的。如果用了-Xms参数，程序在启动的时候就会无条件的从操作系统中挖- Xms后面定义的内存数，然后在这些内存用的差不多的时候，再去挖。
 */
// public native long totalMemory();
static jlong totalMemory(jobject _this)
{
    return g_heap->totalMemory();
}

/* todo
 * maxMemory()这个方法返回的是java虚拟机（这个进程）能构从操作系统那里挖到的最大的内存，以字节为单位，如果在运行java程序的时 候，没有添加-Xmx参数，那么就是64兆，也就是说maxMemory()返回的大约是64*1024*1024字节，这是java虚拟机默认情况下能 从操作系统那里挖到的最大的内存。如果添加了-Xmx参数，将以这个参数后面的值为准，例如java -cp ClassPath -Xmx512m ClassName，那么最大内存就是512*1024*0124字节。
 */
// public native long maxMemory();
static jlong maxMemory(jobject _this)
{
    return VM_HEAP_SIZE;
}

// public native void gc();
static void gc(jobject _this)
{
    JVM_PANIC("gc");
}

/* Wormhole for calling java.lang.ref.Finalizer.runFinalization */
// private static native void runFinalization0();
static void runFinalization0(jobject _this)
{
    JVM_PANIC("runFinalization0");
}

// public native void traceInstructions(boolean on)
static void traceInstructions(jobject _this, jboolean on)
{
    JVM_PANIC("traceInstructions");
}

// public native void traceMethodCalls(boolean on);
static void traceMethodCalls(jobject _this, jboolean on)
{
    JVM_PANIC("traceMethodCalls");
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
