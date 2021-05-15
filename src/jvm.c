#include <assert.h>
#include <time.h>
#include "cabin.h"
#include "jni.h"
#include "heap.h"
#include "symbol.h"
#include "hash.h"


#define JVM_MIRROR(_jclass) ((jclsRef) _jclass)->jvm_mirror

/*
 * This file contains additional functions exported from the VM.
 * These functions are complementary to the standard JNI support.
 * There are three parts to this file:
 *
 * First, this file contains the VM-related functions needed by native
 * libraries in the standard Java API. For example, the java.lang.Object
 * class needs VM-level functions that wait for and notify monitors.
 *
 * Second, this file contains the functions and constant definitions
 * needed by the byte code verifier and class file format checker.
 * These functions allow the verifier and format checker to be written
 * in a VM-independent way.
 *
 * Third, this file contains various I/O and nerwork operations needed
 * by the standard Java I/O and network APIs.
 */

/*
 * Bump the version number when either of the following happens:
 *
 * 1. There is a change in JVM_* functions.
 *
 * 2. There is a change in the contract between VM and Java classes.
 *    For example, if the VM relies on a new private field in Thread
 *    class.
 */

#define JVM_INTERFACE_VERSION 6

JNIEXPORT jint JNICALL
JVM_GetInterfaceVersion(void)
{
    TRACE("JVM_GetInterfaceVersion()");
    return JVM_INTERFACE_VERSION;
}

/*************************************************************************
 PART 1: Functions for Native Libraries
 ************************************************************************/
/*
 * java.lang.Object
 */
JNIEXPORT jint JNICALL
JVM_IHashCode(JNIEnv *env, jobject obj)
{
    TRACE("JVM_IHashCode(env=%p, obj=%p)", env, obj);
    return (jint)(intptr_t)obj; // todo 实现错误。改成当前的时间如何。
}

JNIEXPORT void JNICALL
JVM_MonitorWait(JNIEnv *env, jobject obj, jlong ms)
{
    TRACE("JVM_MonitorWait(env=%p, obj=%p, ms=%ld)", env, obj, ms);
    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_MonitorNotify(JNIEnv *env, jobject obj)
{
    TRACE("JVM_MonitorNotify(env=%p, obj=%p)", env, obj);
    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_MonitorNotifyAll(JNIEnv *env, jobject obj)
{
    TRACE("JVM_MonitorNotifyAll(env=%p, obj=%p)", env, obj);
    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL
JVM_Clone(JNIEnv *env, jobject _obj)
{
    TRACE("JVM_Clone(env=%p, obj=%p)", env, _obj);
    jref obj = (jref) _obj;
    if (!is_subclass_of(obj->clazz, load_boot_class(S(java_lang_Cloneable)))) {
        JNI_THROW(env, S(java_lang_CloneNotSupportedException), NULL); // todo msg
    }
    return (jobject) clone_object(obj);
}

/*
 * java.lang.String
 */
JNIEXPORT jstring JNICALL 
JVM_InternString(JNIEnv *env, jstring str)
{
    TRACE("JVM_InternString(env=%p, str=%p)", env, str);
    assert(g_string_class == ((jstrRef) str)->clazz);
    return (jstring) intern_string((jstrRef) str);
}

/*
 * java.lang.System
 */
JNIEXPORT jlong JNICALL
JVM_CurrentTimeMillis(JNIEnv *env, jclass ignored)
{
    TRACE("JVM_CurrentTimeMillis(env=%p, ignored=%p)", env, ignored);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * 返回最准确的可用系统计时器的当前值，以毫微秒为单位。
 * 此方法只能用于测量已过的时间，与系统或钟表时间的其他任何时间概念无关。
 * 返回值表示从某一固定但任意的时间算起的毫微秒数（或许从以后算起，所以该值可能为负）。
 * 此方法提供毫微秒的精度，但不是必要的毫微秒的准确度。它对于值的更改频率没有作出保证。
 * 在取值范围大于约 292 年（263 毫微秒）的连续调用的不同点在于：由于数字溢出，将无法准确计算已过的时间。
 */
JNIEXPORT jlong JNICALL
JVM_NanoTime(JNIEnv *env, jclass ignored)
{
    TRACE("JVM_NanoTime(env=%p)", env);
    // todo

    struct timespec _time;
    clock_gettime(CLOCK_REALTIME, &_time);  //获取相对于1970到现在的秒数
    return _time.tv_nsec;
}

JNIEXPORT jlong JNICALL
JVM_GetNanoTimeAdjustment(JNIEnv *env, jclass ignored, jlong offset_secs)
{
    TRACE("JVM_GetNanoTimeAdjustment(env=%p, offset_secs=%lld)", env, offset_secs);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_ArrayCopy(JNIEnv *env, jclass ignored, jobject _src, jint src_pos,
              jobject _dst, jint dst_pos, jint length)
{
    TRACE("JVM_ArrayCopy(env=%p, ignored=%p, src=%p, src_pos=%d, dst=%p, dst_pos=%d, length=%d)", 
                    env, ignored, _src, src_pos, _dst, dst_pos, length);

    jarrRef dst = (jarrRef) _dst;
    jarrRef src = (jarrRef) _src;
    assert(is_array_object(dst));
    assert(is_array_object(src));
    array_copy(dst, dst_pos, src, src_pos, length);
}

/*
 * Gather the VM and command line properties and return as a String[].
 * The array indices are alternating key/value pairs
 * supplied by the VM including those defined on the command line
 * using -Dkey=value that may override the platform defined value.
 *
 * Note: The platform encoding must have been set.
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetProperties(JNIEnv *env)
{
    TRACE("JVM_GetProperties(env=%p)", env);

    jarrRef prop_array = alloc_string_array(g_properties_count*2);
    int j = 0;
    for (int i = 0; i < g_properties_count; i++) {
        array_set_ref(prop_array, j++, alloc_string(g_properties[i].name));
        array_set_ref(prop_array, j++, alloc_string(g_properties[i].value));
    }
    return (jobjectArray) prop_array;
}

// JNIEXPORT jobject JNICALL
// JVM_InitProperties(JNIEnv *env, jobject props)
// {
//     TRACE("JVM_InitProperties(env=%p, props=%p)", env, props);
//     // todo init
//     Method *setProperty = lookup_inst_method(((jref) props)->clazz,
//             "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

//     for (int i = 0; i < g_properties_count; i++) {
//         assert(g_properties[i].name != NULL && g_properties[i].value!= NULL);
//         exec_java_func3(setProperty, props, alloc_string(g_properties[i].name), alloc_string(g_properties[i].value));
//     }

//     return props;
// }

// /*
//  * java.io.File
//  */
// JNIEXPORT void JNICALL
// JVM_OnExit(void (*func)(void))
// {
//     TRACE("JVM_OnExit(func=%p)", func);
//     JVM_PANIC("unimplemented"); // todo
// }

// /*
//  * java.lang.Runtime
//  */
// JNIEXPORT void JNICALL
// JVM_Exit(jint code)
// {
//     TRACE("JVM_Exit(code=%d)", code);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * java.lang.Runtime
 */

/*
 * Notify the VM that it's time to halt.
 *
 * static native void beforeHalt();
 */
JNIEXPORT void JNICALL
JVM_BeforeHalt()
{
    TRACE("JVM_BeforeHalt");
    JVM_PANIC("unimplemented"); // todo
}

/*
 * The halt method is synchronized on the halt lock
 * to avoid corruption of the delete-on-shutdown file list.
 * It invokes the true native halt method.
 *
 * static native void halt0(int status);
 */
JNIEXPORT void JNICALL
JVM_Halt(jint code)
{
    TRACE("JVM_Halt(code=%d)", code);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_GC(void)
{
    TRACE("JVM_GC()");
    JVM_PANIC("unimplemented"); // todo
}

/* Returns the number of real-time milliseconds that have elapsed since the
 * least-recently-inspected heap object was last inspected by the garbage
 * collector.
 *
 * For simple stop-the-world collectors this value is just the time
 * since the most recent collection.  For generational collectors it is the
 * time since the oldest generation was most recently collected.  Other
 * collectors are free to return a pessimistic estimate of the elapsed time, or
 * simply the time since the last full collection was performed.
 *
 * Note that in the presence of reference objects, a given object that is no
 * longer strongly reachable may have to be inspected multiple times before it
 * can be reclaimed.
 */
JNIEXPORT jlong JNICALL
JVM_MaxObjectInspectionAge(void)
{
    TRACE("JVM_MaxObjectInspectionAge()");
    JVM_PANIC("unimplemented"); // todo
}

// JNIEXPORT void JNICALL
// JVM_TraceInstructions(jboolean on)
// {
//     TRACE("JVM_TraceInstructions(on=%d)", on);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT void JNICALL
// JVM_TraceMethodCalls(jboolean on)
// {
//     TRACE("JVM_TraceMethodCalls(on=%d)", on);
//     JVM_PANIC("unimplemented"); // todo
// }

/* todo
 * totalMemory()这个方法返回的是java虚拟机现在已经从操作系统那里挖过来的内存大小，也就是java虚拟机这个进程当时所占用的所有 内存。如果在运行java的时候没有添加-Xms参数，那么，在java程序运行的过程的，内存总是慢慢的从操作系统那里挖的，基本上是用多少挖多少，直 挖到maxMemory()为止，所以totalMemory()是慢慢增大的。如果用了-Xms参数，程序在启动的时候就会无条件的从操作系统中挖- Xms后面定义的内存数，然后在这些内存用的差不多的时候，再去挖。
 */
JNIEXPORT jlong JNICALL
JVM_TotalMemory(void)
{
    TRACE("JVM_TotalMemory()");
    return g_heap->size;
}

JNIEXPORT jlong JNICALL
JVM_FreeMemory(void)
{
    TRACE("JVM_FreeMemory()");
    return heap_free_memory(g_heap);
}

/* todo
 * maxMemory()这个方法返回的是java虚拟机（这个进程）能构从操作系统那里挖到的最大的内存，以字节为单位，如果在运行java程序的时 候，没有添加-Xmx参数，那么就是64兆，也就是说maxMemory()返回的大约是64*1024*1024字节，这是java虚拟机默认情况下能 从操作系统那里挖到的最大的内存。如果添加了-Xmx参数，将以这个参数后面的值为准，例如java -cp ClassPath -Xmx512m ClassName，那么最大内存就是512*1024*0124字节。
 */
JNIEXPORT jlong JNICALL
JVM_MaxMemory(void)
{
    TRACE("JVM_MaxMemory()");
    return VM_HEAP_SIZE; // todo
}

JNIEXPORT jint JNICALL
JVM_ActiveProcessorCount(void)
{
    TRACE("JVM_ActiveProcessorCount()");
    return processor_number();
}

JNIEXPORT jboolean JNICALL
JVM_IsUseContainerSupport(void)
{
    TRACE("JVM_IsUseContainerSupport");
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void * JNICALL
JVM_LoadLibrary(const char *name)
{
    TRACE("JVM_LoadLibrary(name=%s)", name);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void * JNICALL
JVM_LogLambdaFormInvoker(JNIEnv* env, jstring line)
{
    TRACE("JVM_LogLambdaFormInvoker()");
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_UnloadLibrary(void *handle)
{
    TRACE("JVM_UnloadLibrary(handle=%p)", handle);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void * JNICALL
JVM_FindLibraryEntry(void *handle, const char *name)
{
    TRACE("JVM_FindLibraryEntry(handle=%p, name=%s)", handle, name);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_IsSupportedJNIVersion(jint version)
{
    TRACE("JVM_IsSupportedJNIVersion(version=%d)", version);
    return JVM_INTERFACE_VERSION == version;
}

JNIEXPORT jobjectArray JNICALL
JVM_GetVmArguments(JNIEnv *env)
{
    TRACE("JVM_GetVmArguments(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_InitializeFromArchive(JNIEnv* env, jclass cls)
{
    TRACE("JVM_InitializeFromArchive(env=%p, cls=%p)", env, cls);
    // JVM_PANIC("unimplemented"); // todo

    // todo 这个函数是干嘛的？？？？？
    Class *c = JVM_MIRROR(cls);
    // todo
}

JNIEXPORT void JNICALL
JVM_RegisterLambdaProxyClassForArchiving(JNIEnv* env, jclass caller,
                                         jstring invokedName,
                                         jobject invokedType,
                                         jobject methodType,
                                         jobject implMethodMember,
                                         jobject instantiatedMethodType,
                                         jclass lambdaProxyClass)
{
    TRACE("JVM_RegisterLambdaProxyClassForArchiving(env=%p)", env); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jclass JNICALL
JVM_LookupLambdaProxyClassFromArchive(JNIEnv* env, jclass caller,
                                      jstring invokedName,
                                      jobject invokedType,
                                      jobject methodType,
                                      jobject implMethodMember,
                                      jobject instantiatedMethodType,
                                      jboolean initialize)
{
    TRACE("JVM_LookupLambdaProxyClassFromArchive(env=%p)", env); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_IsCDSDumpingEnabled(JNIEnv* env)
{
    TRACE("JVM_IsCDSDumpingEnabled(env=%p)", env); 
    // JVM_PANIC("unimplemented"); // todo
    return false; // todo 这个函数是干嘛的？？？？？
}

JNIEXPORT jboolean JNICALL
JVM_IsDynamicDumpingEnabled(JNIEnv* env)
{
    TRACE("JVM_IsDynamicDumpingEnabled(env=%p)", env); 
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_IsSharingEnabled(JNIEnv* env)
{
    TRACE("JVM_IsSharingEnabled(env=%p)", env); 
    // JVM_PANIC("unimplemented"); // todo
    return false; // todo 这个函数是干嘛的？？？？？
}

JNIEXPORT jboolean JNICALL
JVM_IsDumpingClassList(JNIEnv* env)
{
    TRACE("JVM_IsDumpingClassList(env=%p)", env); 

    // JVM_PANIC("unimplemented"); // todo
    return false; // todo 这个函数是干嘛的？？？？？
}

JNIEXPORT jlong JNICALL
JVM_GetRandomSeedForDumping()
{
    TRACE("JVM_GetRandomSeedForDumping()"); 
    // JVM_PANIC("unimplemented"); // todo
    return 100; // todo 这个函数是干嘛的？？？？？
}

// /*
//  * java.lang.Float and java.lang.Double
//  */
// JNIEXPORT jboolean JNICALL
// JVM_IsNaN(jdouble d)
// {
//     TRACE("JVM_IsNaN(d=%f)", d);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * java.lang.Throwable
 */
JNIEXPORT void JNICALL
JVM_FillInStackTrace(JNIEnv *env, jobject _throwable)
{
    TRACE("JVM_FillInStackTrace(env=%p, throwable=%p)", env, _throwable);

    jref throwable = (jref) _throwable;
    Thread *thread = get_current_thread();

    Frame *frame = thread->top_frame;
    int num = count_stack_frames(thread);
    /*
     * 栈顶两帧正在执行 fillInStackTrace(int) 和 fillInStackTrace() 方法，所以需要跳过这两帧。
     * 这两帧下面的几帧正在执行异常类的构造函数，所以也要跳过。
     * 具体要跳过多少帧数则要看异常类的继承层次。
     *
     * (RuntimeException extends Exception extends Throwable extends Object)
     *
     * 比如一个异常抛出示例
     * java.lang.RuntimeException: BAD!
     * at exception/UncaughtTest.main(UncaughtTest.java:6)
     * at exception/UncaughtTest.foo(UncaughtTest.java:10)
     * at exception/UncaughtTest.bar(UncaughtTest.java:14)
     * at exception/UncaughtTest.bad(UncaughtTest.java:18)
     * at java/lang/RuntimeException.<init>(RuntimeException.java:62)
     * at java/lang/Exception.<init>(Exception.java:66)
     * at java/lang/Throwable.<init>(Throwable.java:265)
     * at java/lang/Throwable.fillInStackTrace(Throwable.java:783)
     * at java/lang/Throwable.fillInStackTrace(Native Method)
     */
    Frame *f = frame->prev->prev;
    num -= 2;

    for (Class *c = throwable->clazz; c != NULL; c = c->super_class) {
        f = f->prev; // jump 执行异常类的构造函数的frame
        num--;
        if (utf8_equals(c->class_name, S(java_lang_Throwable))) {
            break; // 可以了，遍历到 Throwable 就行了，因为现在在执行 Throwable 的 fillInStackTrace 方法。
        }
    }

    jarrRef backtrace = alloc_object_array(num);
    Object **trace = (Object **) backtrace->data;

    Class *c = load_boot_class(S(java_lang_StackTraceElement));
    for (int i = 0; f != NULL; f = f->prev) {
        Object *o = alloc_object(c);
        assert(i < num);
        trace[i++] = o;

        // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber)
        // may be should call <init>, but 直接赋值 is also ok. todo

        jstrRef file_name = f->method->clazz->source_file_name != NULL
                        ? alloc_string(f->method->clazz->source_file_name) 
                        : NULL;
        jstrRef class_name = alloc_string(f->method->clazz->class_name);
        jstrRef method_name = alloc_string(f->method->name);
        jint line_number = get_line_number(f->method, f->reader.pc - 1); // todo why 减1？ 减去opcode的长度

        set_ref_field(o, "fileName", "Ljava/lang/String;", file_name);
        set_ref_field(o, "declaringClass", "Ljava/lang/String;", class_name);
        set_ref_field(o, "methodName", "Ljava/lang/String;", method_name);
        set_int_field(o, "lineNumber", line_number);

        // private transient Class<?> declaringClassObject;
        set_ref_field(o, "declaringClassObject", "Ljava/lang/Class;", c->java_mirror);
    }

    /*
     * Native code saves some indication of the stack backtrace in this slot.
     *
     * private transient Object backtrace;
     */
    set_ref_field(throwable, S(backtrace), S(sig_java_lang_Object), backtrace);

    /*
     * The JVM code sets the depth of the backtrace for later retrieval
     * todo test on jdk15
     * private transient int depth;
     */
    set_int_field(throwable, "depth", backtrace->arr_len);
}

// JNIEXPORT jint JNICALL
// JVM_GetStackTraceDepth(JNIEnv *env, jobject throwable)
// {
//     TRACE("JVM_GetStackTraceDepth(env=%p, throwable=%p)", env, throwable);
//     jarrRef backtrace = get_ref_field((jref) throwable, S(backtrace), S(sig_java_lang_Object));
//     assert(backtrace != NULL);
//     return backtrace->arr_len;
// }

// JNIEXPORT jobject JNICALL
// JVM_GetStackTraceElement(JNIEnv *env, jobject throwable, jint index)
// {
//     TRACE("JVM_GetStackTraceElement(env=%p, throwable=%p, index=%d)", env, throwable, index);
//     jarrRef backtrace = get_ref_field((jref) throwable, S(backtrace), S(sig_java_lang_Object));
//     assert(backtrace != NULL);
//     return array_get(jref, backtrace, index);
// }

/*
 * java.lang.StackTraceElement
 */

// Sets the given stack trace elements with the backtrace of the given Throwable.
JNIEXPORT void JNICALL
JVM_InitStackTraceElementArray(JNIEnv *env, jobjectArray _elements, jobject throwable)
{
    TRACE("JVM_InitStackTraceElementArray(env=%p, elements=%p, throwable=%p)", env, _elements, throwable);
    jarrRef elements = (jarrRef) _elements;
    jref x = (jref) throwable;

    jref backtrace = get_ref_field(x, S(backtrace), S(sig_java_lang_Object));
    if (!is_array_object(backtrace)) {
        JVM_PANIC("error"); // todo
    }

    assert(elements->arr_len <= backtrace->arr_len);
    memcpy(elements->data, backtrace->data, elements->arr_len*sizeof(jref));
}

// Sets the given stack trace element with the given StackFrameInfo
JNIEXPORT void JNICALL
JVM_InitStackTraceElement(JNIEnv* env, jobject element, jobject stackFrameInfo)
{
    TRACE("JVM_InitStackTraceElement(env=%p, elements=%p, throwable=%p)", env, element, stackFrameInfo); 
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.lang.NullPointerException
 */

/**
 * Get an extended exception message. This returns a string describing
 * the location and cause of the exception. It returns null for
 * exceptions where this is not applicable.
 *
 * private native String getExtendedNPEMessage();
 */
JNIEXPORT jstring JNICALL
JVM_GetExtendedNPEMessage(JNIEnv *env, jthrowable throwable)
{
    TRACE("JVM_GetExtendedNPEMessage(env=%p, throwable=%p)", env, throwable); 
    JVM_PANIC("unimplemented"); // todo
}


// /*
//  * java.lang.Compiler
//  */
// JNIEXPORT void JNICALL
// JVM_InitializeCompiler (JNIEnv *env, jclass compCls)
// {
//     TRACE("JVM_InitializeCompiler(env=%p, compCls=%p)", env, compCls);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jboolean JNICALL
// JVM_IsSilentCompiler(JNIEnv *env, jclass compCls)
// {
//     TRACE("JVM_IsSilentCompiler(env=%p, compCls=%p)", env, compCls);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jboolean JNICALL
// JVM_CompileClass(JNIEnv *env, jclass compCls, jclass cls)
// {
//     TRACE("JVM_CompileClass(env=%p, compCls=%p, cls=%p)", env, compCls, cls);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jboolean JNICALL
// JVM_CompileClasses(JNIEnv *env, jclass cls, jstring jname)
// {
//     TRACE("JVM_CompileClasses(env=%p, cls=%p, jname=%p)", env, cls, jname);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jobject JNICALL
// JVM_CompilerCommand(JNIEnv *env, jclass compCls, jobject arg)
// {
//     TRACE("JVM_CompilerCommand(env=%p, compCls=%p, arg=%p)", env, compCls, arg);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT void JNICALL
// JVM_EnableCompiler(JNIEnv *env, jclass compCls)
// {
//     TRACE("JVM_EnableCompiler(env=%p, compCls=%p)", env, compCls);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT void JNICALL
// JVM_DisableCompiler(JNIEnv *env, jclass compCls)
// {
//     TRACE("JVM_DisableCompiler(env=%p, compCls=%p)", env, compCls);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * java.lang.StackWalker
 */
enum {
    JVM_STACKWALK_FILL_CLASS_REFS_ONLY       = 0x2,
    JVM_STACKWALK_GET_CALLER_CLASS           = 0x04,
    JVM_STACKWALK_SHOW_HIDDEN_FRAMES         = 0x20,
    JVM_STACKWALK_FILL_LIVE_STACK_FRAMES     = 0x100
};

JNIEXPORT jobject JNICALL
JVM_CallStackWalk(JNIEnv *env, jobject stackStream, jlong mode,
                  jint skip_frames, jint frame_count, jint start_index,
                  jobjectArray frames)
{
    TRACE("JVM_CallStackWalk(env=%p, stackStream=%p)", env, stackStream); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL
JVM_MoreStackWalk(JNIEnv *env, jobject stackStream, jlong mode, jlong anchor,
                  jint frame_count, jint start_index,
                  jobjectArray frames)
{
    TRACE("JVM_MoreStackWalk(env=%p, stackStream=%p)", env, stackStream); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.lang.Thread
 */
static void *threadRunFunc(void *thread)
{
    Method *run_method 
            = lookup_inst_method(load_boot_class(S(java_lang_Thread)), S(run), S(___V));

    jref o = (jref) thread;
    create_thread(o, THREAD_NORM_PRIORITY); // todo
    return (void *) exec_java(run_method, (slot_t[]) { rslot(o) });
}

JNIEXPORT void JNICALL
JVM_StartThread(JNIEnv *env, jobject thread)
{
    TRACE("JVM_StartThread(env=%p, thread=%p)", env, thread);
    // createCustomerThread(_this);

    pthread_t th; 
    if (pthread_create(&th, NULL, threadRunFunc, thread) != 0) {
        // todo error
        JVM_PANIC("pthread_create");
    }
}

JNIEXPORT void JNICALL
JVM_StopThread(JNIEnv *env, jobject thread, jobject exception)
{
    TRACE("JVM_StopThread(env=%p, thread=%p, exception=%p)", env, thread, exception);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Tests if this thread is alive. A thread is alive if it has
 * been started and has not yet died.
 */
JNIEXPORT jboolean JNICALL
JVM_IsThreadAlive(JNIEnv *env, jobject thread)
{
    TRACE("JVM_IsThreadAlive(env=%p, thread=%p)", env, thread);
    return false; // todo 为什么要设置成0，设置成1就状态错误
}

JNIEXPORT void JNICALL
JVM_SuspendThread(JNIEnv *env, jobject thread)
{
    TRACE("JVM_SuspendThread(env=%p, thread=%p)", env, thread);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_ResumeThread(JNIEnv *env, jobject thread)
{
    TRACE("JVM_ResumeThread(env=%p, thread=%p)", env, thread);
    JVM_PANIC("unimplemented"); // todo
}

/**
 * Changes the priority of this thread.
 * <p>
 * First the <code>checkAccess</code> method of this thread is called
 * with no arguments. This may result in throwing a
 * <code>SecurityException</code>.
 * <p>
 * Otherwise, the priority of this thread is set to the smaller of
 * the specified <code>newPriority</code> and the maximum permitted
 * priority of the thread's thread group.
 *
 * @param newPriority priority to set this thread to
 * @exception  IllegalArgumentException  If the priority is not in the
 *               range <code>MIN_PRIORITY</code> to
 *               <code>MAX_PRIORITY</code>.
 * @exception  SecurityException  if the current thread cannot modify
 *               this thread.
 * @see        #getPriority
 * @see        #checkAccess()
 * @see        #getThreadGroup()
 * @see        #MAX_PRIORITY
 * @see        #MIN_PRIORITY
 * @see        ThreadGroup#getMaxPriority()
 */
JNIEXPORT void JNICALL
JVM_SetThreadPriority(JNIEnv *env, jobject thread, jint prio)
{
    TRACE("JVM_SetThreadPriority(env=%p, thread=%p, prio=%d)", env, thread, prio);
    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_Yield(JNIEnv *env, jclass threadClass)
{
    TRACE("JVM_Yield(env=%p, threadClass=%p)", env, threadClass);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_Sleep(JNIEnv *env, jclass threadClass, jlong millis)
{
    TRACE("JVM_Sleep(env=%p, thread=%p, millis=%ld)", env, threadClass, millis);
    if (millis <= 0) {
        JNI_THROW_IllegalArgumentException(env, NULL); // todo msg
    }
    if (millis == 0)
        return;

    JVM_PANIC("unimplemented"); // todo

    // todo
    // this_thread::sleep_for(chrono::microseconds(millis));

    // todo 怎么处理 InterruptedException 
}

// Returns a reference to the currently executing thread Object.
JNIEXPORT jobject JNICALL
JVM_CurrentThread(JNIEnv *env, jclass threadClass)
{
    TRACE("JVM_CurrentThread(env=%p, threadClass=%p)", env, threadClass);
    return (jobject) get_current_thread()->tobj;
}

JNIEXPORT jint JNICALL
JVM_CountStackFrames(JNIEnv *env, jobject thread)
{
    TRACE("JVM_CountStackFrames(env=%p, thread=%p)", env, thread);
    return count_stack_frames(thread_from_tobj((jref) thread));
}

// inform VM of interrupt
JNIEXPORT void JNICALL
JVM_Interrupt(JNIEnv *env, jobject thread)
{
    TRACE("JVM_Interrupt(env=%p, thread=%p)", env, thread);
    Thread *t = thread_from_tobj((jref) thread);
    t->interrupted = true;
}

// /*
//  * Tests if some Thread has been interrupted.  The interrupted state
//  * is reset or not based on the value of ClearInterrupted that is passed.
//  */
// JNIEXPORT jboolean JNICALL
// JVM_IsInterrupted(JNIEnv *env, jobject thread, jboolean clearInterrupted)
// {
//     TRACE("JVM_IsInterrupted(env=%p, thread=%p, clearInterrupted=%d)", env, thread, clearInterrupted);
//     Thread *t = thread_from_tobj(thread);
//     jbool b = t->interrupted;
//     if (b && clearInterrupted) {
//         t->interrupted = jfalse;
//     }
//     return b; 
// }

JNIEXPORT jboolean JNICALL
JVM_HoldsLock(JNIEnv *env, jclass threadClass, jobject obj)
{
    TRACE("JVM_HoldsLock(env=%p, threadClass=%p, obj=%p)", env, threadClass, obj);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_DumpAllStacks(JNIEnv *env, jclass unused)
{
    TRACE("JVM_DumpAllStacks(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobjectArray JNICALL
JVM_GetAllThreads(JNIEnv *env, jclass dummy)
{
    TRACE("JVM_GetAllThreads(env=%p, dummy=%p)", env, dummy);
    int size = g_all_threads_count;
    jarrRef threads = alloc_array0(BOOT_CLASS_LOADER, S(array_java_lang_Thread), size);

    for (int i = 0; i < size; i++) {
        array_set_ref(threads, i, g_all_threads[i]->tobj);
    }

    return (jobjectArray) threads;
}

JNIEXPORT void JNICALL
JVM_SetNativeThreadName(JNIEnv *env, jobject jthread, jstring name)
{
    TRACE("JVM_SetNativeThreadName(env=%p, jthread=%p, name=%p)", env, jthread, name);
    JVM_PANIC("unimplemented"); // todo
}

/* getStackTrace() and getAllStackTraces() method */
JNIEXPORT jobjectArray JNICALL
JVM_DumpThreads(JNIEnv *env, jclass threadClass, jobjectArray _threads)
{
    TRACE("JVM_DumpThreads(env=%p, threadClass=%p, threads=%p)", env, threadClass, _threads);
    jarrRef threads = (jarrRef) (_threads);
    assert(is_array_object(threads));

    size_t len = threads->arr_len;
    jarrRef result = alloc_array0(BOOT_CLASS_LOADER, "[[java/lang/StackTraceElement", len);

    for (size_t i = 0; i < len; i++) {
        jref jThread = array_get(jref, threads, i);
        Thread *thread = thread_from_tobj(jThread);
        jarrRef arr = dump_thread(thread, -1);
        array_set_ref(result, i, arr);
    }

    return (jobjectArray) result;
}

/*
 * java.lang.SecurityManager
 */
// JNIEXPORT jclass JNICALL
// JVM_CurrentLoadedClass(JNIEnv *env)
// {
//     TRACE("JVM_CurrentLoadedClass(env=%p)", env);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jobject JNICALL
// JVM_CurrentClassLoader(JNIEnv *env)
// {
//     TRACE("JVM_CurrentClassLoader(env=%p)", env);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jobjectArray JNICALL
// JVM_EnqueueOperation(JNIEnv *env)
// {
//     // ------------- todo 参数和返回值都不对 --------------------------------------------------------------------------------- --------------------------------------------------------------------------------
//     TRACE("JVM_EnqueueOperation(env=%p)", env);
//     JVM_PANIC("unimplemented"); // todo
// }

JNIEXPORT jobjectArray JNICALL
JVM_GetClassContext(JNIEnv *env)
{
    TRACE("JVM_GetClassContext(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

// JNIEXPORT jint JNICALL
// JVM_ClassDepth(JNIEnv *env, jstring name)
// {
//     TRACE("JVM_ClassDepth(env=%p, name=%p)", env, name);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jint JNICALL
// JVM_ClassLoaderDepth(JNIEnv *env)
// {
//     TRACE("JVM_ClassLoaderDepth(env=%p)", env);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * java.lang.Package
 */
JNIEXPORT jstring JNICALL
JVM_GetSystemPackage(JNIEnv *env, jstring name)
{
    TRACE("JVM_GetSystemPackage(env=%p, name=%p)", env, name);
    const char *utf8_name = string_to_utf8((jstrRef) name);

    const char *pkg = get_boot_package(utf8_name);
    if (pkg == NULL) {
        return NULL;
    } else {
        return (jstring) alloc_string(pkg);
    }
}

JNIEXPORT jobjectArray JNICALL
JVM_GetSystemPackages(JNIEnv *env)
{
    TRACE("JVM_GetSystemPackages(env=%p)", env);
    PHS *packages = get_boot_packages();
    int size = phs_size(packages);
    
    jarrRef ao = alloc_string_array(size);
    Object **p = (Object **) ao->data;
    PHS_TRAVERSAL(packages, const char *, pkg, {
        *p++ = alloc_string(pkg);
    });

    return (jobjectArray) ao;
}

/*
 * java.lang.ref.Reference
 */
JNIEXPORT jobject JNICALL
JVM_GetAndClearReferencePendingList(JNIEnv *env)
{
    TRACE("JVM_GetAndClearReferencePendingList(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_HasReferencePendingList(JNIEnv *env)
{
    TRACE("JVM_HasReferencePendingList(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_WaitForReferencePendingList(JNIEnv *env)
{
    TRACE("JVM_WaitForReferencePendingList(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_ReferenceRefersTo(JNIEnv *env, jobject _ref, jobject _o)
{
    TRACE("JVM_ReferenceRefersTo(env=%p, ref=%p, o=%p)", env, _ref, _o);

    jref ref = (jref) _ref;
    jref o = (jref) _o;

    // todo
    return true;

    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_ReferenceClear(JNIEnv *env, jobject ref)
{
    TRACE("JVM_ReferenceClear(env=%p)", env); // todo

    // todo

    // JVM_PANIC("unimplemented"); // todo
}

/*
 * java.lang.ref.PhantomReference
 */
JNIEXPORT jboolean JNICALL
JVM_PhantomReferenceRefersTo(JNIEnv *env, jobject ref, jobject o)
{
    TRACE("JVM_PhantomReferenceRefersTo(env=%p)", env); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.io.ObjectInputStream
 */
// JNIEXPORT jobject JNICALL
// JVM_AllocateNewObject(JNIEnv *env, jobject obj, jclass currClass, jclass initClass)
// {
//     TRACE("JVM_AllocateNewObject(env=%p, obj=%p, currClass=%p, initClass=%p)", env, obj, currClass, initClass);
//     JVM_PANIC("unimplemented"); // todo
// }

// JNIEXPORT jobject JNICALL
// JVM_AllocateNewArray(JNIEnv *env, jobject obj, jclass currClass, jint length)
// {
//     TRACE("JVM_AllocateNewArray(env=%p, obj=%p, currClass=%p, length=%d)", env, obj, currClass, length);
//     JVM_PANIC("unimplemented"); // todo
// }

JNIEXPORT jobject JNICALL
JVM_LatestUserDefinedLoader(JNIEnv *env)
{
    TRACE("JVM_LatestUserDefinedLoader(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

// /*
//  * This function has been deprecated and should not be considered
//  * part of the specified JVM interface.
//  */
// JNIEXPORT jclass JNICALL
// JVM_LoadClass0(JNIEnv *env, jobject obj, jclass currClass, jstring currClassName)
// {
//     TRACE("JVM_LoadClass0(env=%p, obj=%p, currClass=%p, currClassName=%p)", 
//                     env, obj, currClass, currClassName);
//     JVM_PANIC("This function has been deprecated "
//             "and should not be considered part of the specified JVM interface.");                
// }

/*
 * java.lang.reflect.Array
 */
JNIEXPORT jint JNICALL
JVM_GetArrayLength(JNIEnv *env, jobject arr)
{
    TRACE("JVM_GetArrayLength(env=%p, array=%p)", env, arr);
    jref array = (jref) arr;
    if (array == NULL) {
        JNI_THROW_NPE(env, NULL);
    }
    if (!is_array_object(array)) {
        JNI_THROW_IllegalArgumentException(env, "Argument is not an array");
    }
    return array->arr_len;
}

JNIEXPORT jobject JNICALL
JVM_GetArrayElement(JNIEnv *env, jobject arr, jint index)
{
    TRACE("JVM_GetArrayElement(env=%p, arr=%p, index=%d)", env, arr, index);

    jarrRef array = (jarrRef) arr;
    if (array == NULL) {
        JNI_THROW_NPE(env, NULL);
    }
    if (!is_array_object(array)) {
        JNI_THROW_IllegalArgumentException(env, "Argument is not an array");
    }

    if (index < 0 || index >= array->arr_len) {
        JNI_THROW_ArrayIndexOutOfBoundsException(env, NULL); // todo msg
    }

    switch (array->clazz->class_name[1]) {
    case 'Z': // boolean[]
        return (jobject) bool_box(array_get(jboolean, array, index));
    case 'B': // byte[]
        return (jobject) byte_box(array_get(jbyte, array, index));
    case 'C': // char[]
        return (jobject) char_box(array_get(jchar, array, index));
    case 'S': // short[]
        return (jobject) short_box(array_get(jshort, array, index));
    case 'I': // int[]
        return (jobject) int_box(array_get(jint, array, index));
    case 'J': // long[]
        return (jobject) long_box(array_get(jlong, array, index));
    case 'F': // float[]
        return (jobject) float_box(array_get(jfloat, array, index));
    case 'D': // double[]
        return (jobject) double_box(array_get(jdouble, array, index));
    default:  // reference array
        return (jobject) array_get(jref, array, index);
    }
}

JNIEXPORT jvalue JNICALL
JVM_GetPrimitiveArrayElement(JNIEnv *env, jobject arr, jint index, jint wCode)
{
    TRACE("JVM_GetPrimitiveArrayElement(env=%p, arr=%p, index=%d, wCode=%d)", env, arr, index, wCode);
    // if (wCode == JVM_AT_BOOLEAN) {

    // }
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_SetArrayElement(JNIEnv *env, jobject arr, jint index, jobject val)
{
    TRACE("JVM_SetArrayElement(env=%p, arr=%p, index=%d, val=%p)", env, arr, index, val);
    jarrRef array = (jarrRef) arr;
    jref value = (jref) val;

    if (array == NULL) {
        JNI_THROW_NPE(env, NULL);
    }
    if (!is_array_object(array)) {
        JNI_THROW_IllegalArgumentException(env, "Argument is not an array");
    }

    if (index < 0 || index >= array->arr_len) {
        JNI_THROW_NegativeArraySizeException(env, NULL); // todo msg
    }

    if (is_prim_array(array) && value == NULL) {
        // 基本类型的数组无法设空值
        JNI_THROW_IllegalArgumentException(env, NULL); // todo msg
    }

    switch (array->clazz->class_name[1]) {
    case 'Z': // boolean[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Boolean))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_boolean(array, index, slot_get_bool(prim_wrapper_obj_unbox(value)));
        }
        return;
    case 'B': // byte[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Byte))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_byte(array, index, slot_get_byte(prim_wrapper_obj_unbox(value)));
        }
        return;
    case 'C': // char[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Character))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_char(array, index, slot_get_char(prim_wrapper_obj_unbox(value)));
        } 
        return;
    case 'S': // short[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Short))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_short(array, index, slot_get_short(prim_wrapper_obj_unbox(value)));
        } 
        return;
    case 'I': // int[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Integer))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_int(array, index, slot_get_int(prim_wrapper_obj_unbox(value)));
        } 
        return;    
    case 'J': // long[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Long))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_long(array, index, slot_get_long(prim_wrapper_obj_unbox(value)));
        } 
        return;    
    case 'F': // float[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Float))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_float(array, index, slot_get_float(prim_wrapper_obj_unbox(value)));
        } 
        return;    
    case 'D': // double[]
        if (!utf8_equals(value->clazz->class_name, S(java_lang_Double))) {
            JNI_THROW_IllegalArgumentException(env, "argument type mismatch");
        } else {
            array_set_double(array, index, slot_get_double(prim_wrapper_obj_unbox(value)));
        } 
        return;    
    default:  // reference array
        array_set_ref(array, index, value);
        return;
    }
}

JNIEXPORT void JNICALL
JVM_SetPrimitiveArrayElement(JNIEnv *env, jobject arr, jint index, jvalue v, unsigned char vCode)
{
    TRACE("JVM_SetPrimitiveArrayElement(env=%p, arr=%p, index=%d, v=%lld, vCode=%d)",
                                         env, arr, index, v.j, vCode);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL
JVM_NewArray(JNIEnv *env, jclass _eltClass, jint length)
{
    TRACE("JVM_NewArray(env=%p, eltClass=%p, length=%d)", env, _eltClass, length);

    jclsRef eltClass = (jclsRef) _eltClass;
    if (eltClass == NULL) {
        JNI_THROW_NPE(env, "component type is null");
        return NULL;
    }
    if (length < 0) {
        JNI_THROW_NegativeArraySizeException(env,  NULL);  // todo msg
        return NULL;
    }
    return (jobject) alloc_array(array_class(eltClass->jvm_mirror), length);
}

JNIEXPORT jobject JNICALL
JVM_NewMultiArray(JNIEnv *env, jclass _eltClass, jintArray dim)
{
    TRACE("JVM_NewMultiArray(env=%p, eltClass=%p, length=%p)", env, _eltClass, dim);
    jclsRef eltClass = (jclsRef) _eltClass;
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.lang.Class and java.lang.ClassLoader
 */

/*
 * Returns the immediate caller class of the native method invoking
 * JVM_GetCallerClass.  The Method.invoke and other frames due to
 * reflection machinery are skipped.
 *
 * The caller is expected to be marked with
 * jdk.internal.reflect.CallerSensitive. The JVM will throw an
 * error if it is not marked properly.
 */
JNIEXPORT jclass JNICALL
JVM_GetCallerClass(JNIEnv *env)
{
    TRACE("JVM_GetCallerClass(env=%p)", env);

    // top0, current frame is executing getCallerClass()
    // top1, who called getCallerClass, the one who wants to know his caller.
    // top2, the caller of top1, the result.
    Frame *frame = (Frame *) get_current_thread()->top_frame;
    Frame *top1 = frame->prev;
    assert(top1 != NULL);

    Frame *top2 = top1->prev;
    assert(top2 != NULL);

    jclsRef o = top2->method->clazz->java_mirror;
    return (jclass) o;
}


// /*
//  * Returns the class in which the code invoking the native method
//  * belongs.
//  *
//  * Note that in JDK 1.1, native methods did not create a frame.
//  * In 1.2, they do. Therefore native methods like Class.forName
//  * can no longer look at the current frame for the caller class.
//  */
// JNIEXPORT jclass JNICALL
// JVM_GetCallerClass(JNIEnv *env, int n)
// {
//     TRACE("JVM_GetCallerClass(env=%p, n=%d)", env, n);

//     // n < 0的情况
//     // top0, current frame is executing getCallerClass()
//     // top1, who called getCallerClass, the one who wants to know his caller.
//     // top2, the caller of top1, the result.
//     if (n < 0)
//         n = 2;

//     Frame *frame = (Frame *) get_current_thread()->top_frame;
//     for (; n > 0 && frame != NULL; n--) {
//         frame = frame->prev;
//     }

//     if (frame == NULL) {
//         return NULL;
//     }
//     return frame->method->clazz->java_mirror;    
// }

/*
 * Find primitive classes
 * utf: class name
 */
JNIEXPORT jclass JNICALL
JVM_FindPrimitiveClass(JNIEnv *env, const char *utf)
{
    TRACE("JVM_FindPrimitiveClass(env=%p, utf=%s)", env, utf);
    // 这里的 class name 是诸如 "int, float" 之类的 primitive type
    return (jclass) load_boot_class(utf)->java_mirror;
}

// /*
//  * Link the class
//  */
// JNIEXPORT void JNICALL
// JVM_ResolveClass(JNIEnv *env, jclass cls)
// {
//     TRACE("JVM_ResolveClass(env=%p, cls=%p)", env, cls);
//     JVM_PANIC("unimplemented"); // todo
// }

// /*
//  * Find a class from a given class loader. Throw ClassNotFoundException
//  * or NoClassDefFoundError depending on the value of the last argument.
//  */
// JNIEXPORT jclass JNICALL
// JVM_FindClassFromClassLoader(JNIEnv *env, const char *name, jboolean init,
//                              jobject loader, jboolean throwError)
// {
//     TRACE("JVM_FindClassFromClassLoader(env=%p, name=%s, init=%d, loader=%p, throwError=%d)",
//                                      env, name, init, loader, throwError);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * Find a class from a boot class loader. Returns NULL if class not found.
 */
JNIEXPORT jclass JNICALL
JVM_FindClassFromBootLoader(JNIEnv *env, const char *name)
{
    TRACE("JVM_FindClassFromBootLoader(env=%p, name=%s)", env, name);
    if (name == NULL)
        return NULL;
    Class *c = load_boot_class(name);
    return c != NULL ? (jclass) c->java_mirror : NULL;
}

/*
 * Find a class from a given class loader.  Throws ClassNotFoundException.
 *  name:   name of class
 *  init:   whether initialization is done
 *  loader: class loader to look up the class. This may not be the same as the caller's
 *          class loader.
 *  caller: initiating class. The initiating class may be null when a security
 *          manager is not installed.
 */
JNIEXPORT jclass JNICALL
JVM_FindClassFromCaller(JNIEnv *env, const char *name, jboolean init,
                        jobject loader, jclass caller)
{
    TRACE("JVM_FindClassFromCaller(env=%p)", env); // todo

    // println("class name: %s", name);

    // todo
    Class *c = load_class((jref) loader, name);
    if (c == NULL) {
        // todo ClassNotFoundException
        JVM_PANIC("unimplemented"); // todo
        // (*env)->ThrowNew(env, )
    }

    return (jclass) c->java_mirror;

    // JVM_PANIC("unimplemented"); // todo
}

/*
 * Find a class from a given class.
 */
JNIEXPORT jclass JNICALL
JVM_FindClassFromClass(JNIEnv *env, const char *name, jboolean init, jclass from)
{
    TRACE("JVM_FindClassFromClass(env=%p, name=%s, init=%d, from=%p)",
                                     env, name, init, from);
    JVM_PANIC("unimplemented"); // todo
}

/* Find a loaded class cached by the VM */
JNIEXPORT jclass JNICALL
JVM_FindLoadedClass(JNIEnv *env, jobject loader, jstring name)
{
    TRACE("JVM_FindLoadedClass(env=%p, loader=%p, name=%p)", env, loader, name);
    if (name == NULL)
        return NULL;
    utf8_t *slash_name = dot_to_slash_dup(string_to_utf8((jstrRef) name));
    Class *c = find_loaded_class((jref) loader, slash_name);
    return c != NULL ? (jclass) c->java_mirror : NULL;
}

/* Define a class */
JNIEXPORT jclass JNICALL
JVM_DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len, jobject pd)
{
    TRACE("JVM_DefineClass(env=%p, name=%s, loader=%p, buf=%p, len=%d, pd=%p)", 
                    env, name, loader, buf, len, pd);

    Class *c = define_class((jref) loader, (u1 *) buf, len);
    // c->class_name和name是否相同 todo
//    printvm("class_name: %s\n", c->class_name);
    return (jclass) c->java_mirror;
}

/* Define a class with a source (added in JDK1.5) */
JNIEXPORT jclass JNICALL
JVM_DefineClassWithSource(JNIEnv *env, const char *name, jobject loader,
                          const jbyte *buf, jsize len, jobject pd,
                          const char *source)
{
    TRACE("JVM_DefineClassWithSource(env=%p, name=%s, loader=%p, buf=%p, len=%d, pd=%p, source=%s)", 
                    env, name, loader, buf, len, pd, source);

    Class *c = define_class((jref) loader, (u1 *) buf, len);
    
    // c->class_name和name是否相同 todo
//    printvm("class_name: %s\n", c->class_name);
    // todo 判断 source 与 c->source_file_name 是否相同

    return (jclass) c->java_mirror;
}                         

// /* Define a class with a source with conditional verification (added HSX 14)
//  * -Xverify:all will verify anyway, -Xverify:none will not verify,
//  * -Xverify:remote (default) will obey this conditional
//  * i.e. true = should_verify_class
//  */
// JNIEXPORT jclass JNICALL
// JVM_DefineClassWithSourceCond(JNIEnv *env, const char *name,
//                               jobject loader, const jbyte *buf,
//                               jsize len, jobject pd, const char *source,
//                               jboolean verify)
// {
//     TRACE("JVM_DefineClassWithSourceCond(env=%p, name=%s, loader=%p, buf=%p, "
//                                     "len=%d, pd=%p, source=%s, verify=%d)",
//                                      env, name, loader, buf, len, pd, source, verify);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * Define a class with the specified lookup class.
 *  lookup:  Lookup class
 *  name:    the name of the class
 *  buf:     class bytes
 *  len:     length of class bytes
 *  pd:      protection domain
 *  init:    initialize the class
 *  flags:   properties of the class
 *  classData: private static pre-initialized field; may be null
 */
JNIEXPORT jclass JNICALL
JVM_LookupDefineClass(JNIEnv *env, jclass lookup, const char *name, const jbyte *buf,
                      jsize len, jobject pd, jboolean init, int flags, jobject classData)
{
    TRACE("JVM_LookupDefineClass(env=%p)", env); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Module support funcions
 */

/*
 * Define a module with the specified packages and bind the module to the
 * given class loader.
 *  module:       module to define
 *  is_open:      specifies if module is open (currently ignored)
 *  version:      the module version
 *  location:     the module location
 *  packages:     array of packages in the module
 */
JNIEXPORT void JNICALL
JVM_DefineModule(JNIEnv *env, jobject module, jboolean is_open, jstring version,
                 jstring location, jobjectArray packages)
{
    TRACE("JVM_DefineModule(env=%p)", env); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Set the boot loader's unnamed module.
 *  module: boot loader's unnamed module
 */
JNIEXPORT void JNICALL
JVM_SetBootLoaderUnnamedModule(JNIEnv *env, jobject module)
{
    TRACE("JVM_SetBootLoaderUnnamedModule(env=%p, module=%p)", env, module);

    // todo

    // JVM_PANIC("unimplemented"); // todo
}

/*
 * Do a qualified export of a package.
 *  from_module: module containing the package to export
 *  package:     name of the package to export
 *  to_module:   module to export the package to
 */
JNIEXPORT void JNICALL
JVM_AddModuleExports(JNIEnv *env, jobject from_module, jstring package, jobject to_module)
{
    TRACE("JVM_AddModuleExports(env=%p, from_module=%p)", env, from_module); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Do an export of a package to all unnamed modules.
 *  from_module: module containing the package to export
 *  package:     name of the package to export to all unnamed modules
 */
JNIEXPORT void JNICALL
JVM_AddModuleExportsToAllUnnamed(JNIEnv *env, jobject from_module, jstring package)
{
    TRACE("JVM_AddModuleExportsToAllUnnamed(env=%p, from_module=%p, package=%p)", env, from_module, package); 
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Do an unqualified export of a package.
 *  from_module: module containing the package to export
 *  package:     name of the package to export
 */
JNIEXPORT void JNICALL
JVM_AddModuleExportsToAll(JNIEnv *env, jobject from_module, jstring package)
{
    TRACE("JVM_AddModuleExportsToAll(env=%p, from_module=%p, package=%p)", env, from_module, package); 
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Add a module to the list of modules that a given module can read.
 *  from_module:   module requesting read access
 *  source_module: module that from_module wants to read
 */
JNIEXPORT void JNICALL
JVM_AddReadsModule(JNIEnv *env, jobject from_module, jobject source_module)
{
    TRACE("JVM_AddReadsModule(env=%p, from_module=%p, source_module=%p)", env, from_module, source_module); 
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Define all modules that have been stored in the CDS archived heap.
 *  platform_loader: the built-in platform class loader
 *  system_loader:   the built-in system class loader
 */
JNIEXPORT void JNICALL
JVM_DefineArchivedModules(JNIEnv *env, jobject platform_loader, jobject system_loader)
{
    TRACE("JVM_DefineArchivedModules(env=%p, platform_loader=%p, system_loader=%p)", 
                                    env, platform_loader, system_loader); 
    JVM_PANIC("unimplemented"); // todo
}


/*
 * Reflection support functions
 */

// /*
//  * Returns the name of the entity (class, interface, array class,
//  * primitive type, or void) represented by this Class object, as a String.
//  *
//  * Examples:
//  * String.class.getName()
//  *     returns "java.lang.String"
//  * byte.class.getName()
//  *     returns "byte"
//  * (new Object[3]).getClass().getName()
//  *     returns "[Ljava.lang.Object;"
//  * (new int[3][4][5][6][7][8][9]).getClass().getName()
//  *     returns "[[[[[[[I"
//  */
// JNIEXPORT jstring JNICALL
// JVM_GetClassName(JNIEnv *env, jclass cls)
// {
//     TRACE("JVM_GetClassName(env=%p, cls=%p)", env, cls);

//     jstring name = alloc_string(slash_to_dot_dup(JVM_MIRROR(cls)->class_name));
//     assert(g_string_class != NULL);
//     return intern_string(name);
// }

/*
 * Cache the name to reduce the number of calls into the VM.
 * This field would be set by VM itself during initClassName call.
 * 
 * private transient String name;
 * private native String initClassName();
 */ 
JNIEXPORT jstring JNICALL
JVM_InitClassName(JNIEnv *env, jclass cls)
{
    TRACE("JVM_InitClassName(env=%p, cls=%p)", env, cls);

    jstrRef class_name = alloc_string(slash_to_dot_dup(JVM_MIRROR(cls)->class_name));
    class_name = intern_string(class_name);
    set_ref_field((jclsRef) cls, "name", "Ljava/lang/String;", class_name);
    return (jstring) class_name;
}

/**
 * Determines the interfaces implemented by the class or interface
 * represented by this object.
 *
 * <p> If this object represents a class, the return value is an array
 * containing objects representing all interfaces implemented by the
 * class. The order of the interface objects in the array corresponds to
 * the order of the interface names in the {@code implements} clause
 * of the declaration of the class represented by this object. For
 * example, given the declaration:
 * <blockquote>
 * {@code class Shimmer implements FloorWax, DessertTopping { ... }}
 * </blockquote>
 * suppose the value of {@code s} is an instance of
 * {@code Shimmer}; the value of the expression:
 * <blockquote>
 * {@code s.getClass().getInterfaces()[0]}
 * </blockquote>
 * is the {@code Class} object that represents interface
 * {@code FloorWax}; and the value of:
 * <blockquote>
 * {@code s.getClass().getInterfaces()[1]}
 * </blockquote>
 * is the {@code Class} object that represents interface
 * {@code DessertTopping}.
 *
 * <p> If this object represents an interface, the array contains objects
 * representing all interfaces extended by the interface. The order of the
 * interface objects in the array corresponds to the order of the interface
 * names in the {@code extends} clause of the declaration of the
 * interface represented by this object.
 *
 * <p> If this object represents a class or interface that implements no
 * interfaces, the method returns an array of length 0.
 *
 * <p> If this object represents a primitive type or void, the method
 * returns an array of length 0.
 *
 * <p> If this {@code Class} object represents an array type, the
 * interfaces {@code Cloneable} and {@code java.io.Serializable} are
 * returned in that order.
 *
 * @return an array of interfaces implemented by this class.
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetClassInterfaces(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassInterfaces(env=%p, cls=%p)", env, cls);

    Class *c = JVM_MIRROR(cls);
    jarrRef interfaces = alloc_class_array(c->interfaces_count);
    for (u2 i = 0; i < c->interfaces_count; i++) {
        assert(c->interfaces[i] != NULL);
        array_set_ref(interfaces, i, c->interfaces[i]->java_mirror);
    }

    return (jobjectArray) interfaces;
}

// JNIEXPORT jobject JNICALL
// JVM_GetClassLoader(JNIEnv *env, jclass cls)
// {
//     TRACE("JVM_GetClassLoader(env=%p, cls=%p)", env, cls);
//     return JVM_MIRROR(cls)->loader;
// }

JNIEXPORT jboolean JNICALL
JVM_IsInterface(JNIEnv *env, jclass cls)
{
    TRACE("JVM_IsInterface(env=%p, cls=%p)", env, cls);
    return IS_INTERFACE(JVM_MIRROR(cls)) ? JNI_TRUE : JNI_FALSE;
}

/*
 * Gets the signers of this class.
 *
 * the signers of this class, or null if there are no signers.
 * In particular, this method returns null
 * if this object represents a primitive type or void.
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetClassSigners(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassSigners(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_SetClassSigners(JNIEnv *env, jclass cls, jobjectArray signers)
{
    TRACE("JVM_SetClassSigners(env=%p, cls=%p, signers=%p)", env, cls, signers);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the ProtectionDomain of this class.
 */
// private native java.security.ProtectionDomain getProtectionDomain0();
JNIEXPORT jobject JNICALL
JVM_GetProtectionDomain(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetProtectionDomain(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_IsArrayClass(JNIEnv *env, jclass cls)
{
    TRACE("JVM_IsArrayClass(env=%p, cls=%p)", env, cls);
    return is_array_class(JVM_MIRROR(cls)) ? JNI_TRUE : JNI_FALSE;  // todo
}

JNIEXPORT jboolean JNICALL
JVM_IsPrimitiveClass(JNIEnv *env, jclass cls)
{
    TRACE("JVM_IsPrimitiveClass(env=%p, cls=%p)", env, cls);
    return is_prim_class(JVM_MIRROR(cls)) ? JNI_TRUE : JNI_FALSE;
}

/**
 * Returns {@code true} if and only if the underlying class is a hidden class.
 *
 * @return {@code true} if and only if this class is a hidden class.
 *
 * @since 15
 * @see MethodHandles.Lookup#defineHiddenClass
 * 
 * @HotSpotIntrinsicCandidate
 * public native boolean isHidden();
 * 
 * http://openjdk.java.net/jeps/371
 */
JNIEXPORT jboolean JNICALL
JVM_IsHiddenClass(JNIEnv *env, jclass cls)
{
    TRACE("JVM_IsHiddenClass(env=%p, cls=%p)", env, cls);
    // // todo 什么是HiddenClass？？
    // Class *c = JVM_MIRROR(cls);
    // return false;

    JVM_PANIC("unimplemented"); // todo
}

// /*
//  * Returns the representing the component type of an array.
//  * If this class does not represent an array class this method returns null.
//  *
//  * like:
//  * [[I -> [I -> int -> null
//  */
// JNIEXPORT jclass JNICALL
// JVM_GetComponentType(JNIEnv *env, jclass cls)
// {
//     TRACE("JVM_GetComponentType(env=%p, cls=%p)", env, cls);
//     Class *c = ((jclsRef) cls)->jvm_mirror;
//     if (is_array_class(c)) {
//         return component_class(c)->java_mirror;
//     } else {
//         return NULL;
//     }
// }

JNIEXPORT jint JNICALL
JVM_GetClassModifiers(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassModifiers(env=%p, cls=%p)", env, cls);
    return JVM_MIRROR(cls)->access_flags; // todo
}

/*
 * getClasses和getDeclaredClasses的区别：
 * getClasses得到该类及其父类所有的public的内部类。
 * getDeclaredClasses得到该类所有的内部类，除去父类的。
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetDeclaredClasses(JNIEnv *env, jclass ofClass)
{
    TRACE("JVM_GetDeclaredClasses(env=%p, ofClass=%p)", env, ofClass);
    JVM_PANIC("unimplemented"); // todo
}

/**
 * If the class or interface represented by this {@code Class} object
 * is a member of another class, returns the {@code Class} object
 * representing the class in which it was declared.  This method returns
 * null if this class or interface is not a member of any other class.  If
 * this {@code Class} object represents an array class, a primitive
 * type, or void,then this method returns null.
 *
 * 如果此类为内部类，返回其外部类
 *
 * @return the declaring class for this class
 * @throws SecurityException
 *         If a security manager, <i>s</i>, is present and the caller's
 *         class loader is not the same as or an ancestor of the class
 *         loader for the declaring class and invocation of {@link
 *         SecurityManager#checkPackageAccess s.checkPackageAccess()}
 *         denies access to the package of the declaring class
 * @since JDK1.1
 *
 * private native Class<?> getDeclaringClass0();
 */
JNIEXPORT jclass JNICALL
JVM_GetDeclaringClass(JNIEnv *env, jclass of_class)
{
    TRACE("JVM_GetDeclaringClass(env=%p, of_class=%p)", env, of_class);
    Class *c = JVM_MIRROR(of_class);
    if (is_array_class(c)) {
        return NULL;
    }

    char buf[strlen(c->class_name) + 1];
    strcpy(buf, c->class_name);
    char *last_dollar = strrchr(buf, '$'); // 内部类标识：out_class_name$inner_class_name
    if (last_dollar == NULL) {
        return NULL;
    } 
    
    *last_dollar = 0;
    c = load_class(c->loader, buf);
    assert(c != NULL);
    return (jclass) c->java_mirror;
}

/*
 * Returns the "simple binary name" of the underlying class, i.e.,
 * the binary name without the leading enclosing class name.
 * Returns null if the underlying class is a top level class.
 * 
 * private native String getSimpleBinaryName0();
 */
JNIEXPORT jstring JNICALL
JVM_GetSimpleBinaryName(JNIEnv *env, jclass ofClass)
{
    TRACE("JVM_GetSimpleBinaryName(env=%p, ofClass=%p)", env, ofClass);
    JVM_PANIC("unimplemented"); // todo
}

/* Generics support (JDK 1.5) */
JNIEXPORT jstring JNICALL
JVM_GetClassSignature(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassSignature(env=%p, cls=%p)", env, cls);
    Class *c = JVM_MIRROR(cls);
    if (c->signature != NULL)
        return (jstring) alloc_string(c->signature);
    return NULL;
}

/* Annotations support (JDK 1.5) */
JNIEXPORT jbyteArray JNICALL
JVM_GetClassAnnotations(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassAnnotations(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

// /* Annotations support (JDK 1.6) */

// // field is a handle to a java.lang.reflect.Field object
// JNIEXPORT jbyteArray JNICALL
// JVM_GetFieldAnnotations(JNIEnv *env, jobject field)
// {
//     TRACE("JVM_GetFieldAnnotations(env=%p, field=%p)", env, field);
//     JVM_PANIC("unimplemented"); // todo
// }

// // method is a handle to a java.lang.reflect.Method object
// JNIEXPORT jbyteArray JNICALL
// JVM_GetMethodAnnotations(JNIEnv *env, jobject method)
// {
//     TRACE("JVM_GetMethodAnnotations(env=%p, method=%p)", env, method);
//     JVM_PANIC("unimplemented"); // todo
// }

// // method is a handle to a java.lang.reflect.Method object
// JNIEXPORT jbyteArray JNICALL
// JVM_GetMethodDefaultAnnotationValue(JNIEnv *env, jobject method)
// {
//     TRACE("JVM_GetMethodDefaultAnnotationValue(env=%p, method=%p)", env, method);
//     JVM_PANIC("unimplemented"); // todo
// }

// // method is a handle to a java.lang.reflect.Method object
// JNIEXPORT jbyteArray JNICALL
// JVM_GetMethodParameterAnnotations(JNIEnv *env, jobject method)
// {
//     TRACE("JVM_GetMethodParameterAnnotations(env=%p, method=%p)", env, method);
//     JVM_PANIC("unimplemented"); // todo
// }

/* Type use annotations support (JDK 1.8) */

JNIEXPORT jbyteArray JNICALL
JVM_GetClassTypeAnnotations(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassTypeAnnotations(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

// field is a handle to a java.lang.reflect.Field object
JNIEXPORT jbyteArray JNICALL
JVM_GetFieldTypeAnnotations(JNIEnv *env, jobject field)
{
    TRACE("JVM_GetFieldTypeAnnotations(env=%p, field=%p)", env, field);
    JVM_PANIC("unimplemented"); // todo
}

// method is a handle to a java.lang.reflect.Method object
JNIEXPORT jbyteArray JNICALL
JVM_GetMethodTypeAnnotations(JNIEnv *env, jobject method)
{
    TRACE("JVM_GetMethodTypeAnnotations(env=%p, method=%p)", env, method);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * New (JDK 1.4) reflection implementation
 */

/*
 * 注意 getDeclaredMethods 和 getMethods 方法的不同。
 * getDeclaredMethods(),该方法是获取本类中的所有方法，包括私有的(private、protected、默认以及public)的方法。
 * getMethods(),该方法是获取本类以及父类或者父接口中所有的公共方法(public修饰符修饰的)
 *
 * getDeclaredMethods 强调的是本类中定义的方法，不包括继承而来的。
 * 不包括 class initialization method(<clinit>)和构造函数(<init>)
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetClassDeclaredMethods(JNIEnv *env, jclass _ofClass, jboolean publicOnly)
{
    TRACE("JVM_GetClassDeclaredMethods(env=%p, ofClass=%p, publicOnly=%d)", env, _ofClass, publicOnly);

    jclsRef ofClass = (jclsRef) _ofClass;
    Class *cls = ofClass->jvm_mirror;
    // jint count = public_only ? cls->public_methods_count : cls->methods.size();

    Class *method_class = load_boot_class(S(java_lang_reflect_Method));

    /*
     * Method(Class<?> declaringClass, String name, Class<?>[] parameterTypes, Class<?> returnType,
     *      Class<?>[] checkedExceptions, int modifiers, int slot, String signature,
     *      byte[] annotations, byte[] parameterAnnotations, byte[] annotationDefault)
     */
    Method *constructor = get_constructor(method_class, "(Ljava/lang/Class;" "Ljava/lang/String;" "[" "Ljava/lang/Class;" "Ljava/lang/Class;" "[" "Ljava/lang/Class;" "II" "Ljava/lang/String;" "[B[B[B)V");

    u2 count = 0;
    Object *objects[cls->methods_count];

    for (int i = 0; i < cls->methods_count; i++) {
        Method *method = cls->methods + i;
        if (publicOnly && !IS_PUBLIC(method))
            continue;
        if ((strcmp(method->name, "<clinit>") == 0) || (strcmp(method->name, "<init>") == 0))
            continue;

        Object *o = alloc_object(method_class);
        objects[count++] = o;
        // Object *o = alloc_object(method_class);
        // methods.push_back(o);

        exec_java(constructor, (slot_t []) {
                rslot(o),        /* this  */
                rslot(ofClass), /* declaring class */
                // name must be interned.
                // 参见 java/lang/reflect/Method 的说明
                rslot(intern_string(alloc_string(method->name))), /* name */
                rslot(get_parameter_types(method)), /* parameter types */
                rslot(get_return_type(method)),     /* return type */
                rslot(get_exception_types(method)), /* checked exceptions */
                islot(method->access_flags), /* modifiers todo */
                islot(i), /* slot   todo */
                rslot(method->signature != NULL ? alloc_string(method->signature) : NULL), /* signature  todo */
                rslot(NULL), /* annotations  todo */
                rslot(NULL), /* parameter annotations  todo */
                rslot(NULL), /* annotation default  todo */
        });
    }
        
    jarrRef method_array = alloc_array(array_class(method_class), count);
    for (size_t i = 0; i < count; i++) {
        array_set_ref(method_array, i, objects[i]);
    }
    return (jobjectArray) method_array;
}

JNIEXPORT jobjectArray JNICALL
JVM_GetClassDeclaredFields(JNIEnv *env, jclass _ofClass, jboolean publicOnly)
{
    TRACE("JVM_GetClassDeclaredFields(env=%p, ofClass=%p, publicOnly=%d)", env, _ofClass, publicOnly);
    
    jclsRef ofClass = (jclsRef) _ofClass;
    Class *cls = ofClass->jvm_mirror;
    // jint count = public_only ? cls->public_fields_count : cls->fields.size();

    Class *field_class = load_boot_class(S(java_lang_reflect_Field));
    // auto field_array = field_class->arrayClass()->allocArray(count);

    Method *constructor;
    // Field(Class<?> declaringClass, String name, Class<?> type, int modifiers,
    //     boolean trustedFinal, int slot, String signature, byte[] annotations)
    constructor = get_constructor(field_class, "(Ljava/lang/Class;" "Ljava/lang/String;" "Ljava/lang/Class;" "IZI" "Ljava/lang/String;" "[B)V");

    u2 count = 0;
    Object *objects[cls->fields_count];

    // invoke constructor of class java/lang/reflect/Field
    for (int i = 0; i < cls->fields_count; i++) {
        Field *f = cls->fields + i;
        if (publicOnly && !IS_PUBLIC(f))
            continue;

        Object *o = alloc_object(field_class);
        objects[count++] = o;
        // field_array->setRef(i, o);

        exec_java(constructor, (slot_t []) {
                rslot(o), // this
                rslot(ofClass), // declaring class
                // name must be interned.
                // 参见 java/lang/reflect/Field 的说明
                rslot(intern_string(alloc_string(f->name))), // name
                rslot(get_field_type(f)), // type
                islot(f->access_flags), /* modifiers todo */
                islot(IS_FINAL(f) ? jtrue : jfalse), // todo trusted Final
                islot(f->id), /* slot   todo */
                rslot(f->signature != NULL ? alloc_string(f->signature) : NULL), /* signature  todo */
                rslot(NULL), /* annotations  todo */
        });
    }

    jarrRef field_array = alloc_array(array_class(field_class), count);
    for (u2 i = 0; i < count; i++)
        array_set_ref(field_array, i, objects[i]);

    return (jobjectArray) field_array;
}

JNIEXPORT jobjectArray JNICALL
JVM_GetClassDeclaredConstructors(JNIEnv *env, jclass _ofClass, jboolean publicOnly)
{
    TRACE("JVM_GetClassDeclaredConstructors(env=%p, ofClass=%p, publicOnly=%d)", env, _ofClass, publicOnly);
    
    jclsRef ofClass = (jclsRef) _ofClass;
    Class *cls = ofClass->jvm_mirror;

    // std::vector<Method *> constructors = get_constructors(cls, public_only);
    // int count = constructors.size();
    int count;
    Method **constructors = get_constructors(cls, publicOnly, &count);

   Class *constructor_class = load_boot_class("java/lang/reflect/Constructor");
   jarrRef constructor_array = alloc_array(array_class(constructor_class), count);

   /*
     * Constructor(Class<T> declaringClass, Class<?>[] parameterTypes,
     *      Class<?>[] checkedExceptions, int modifiers, int slot,
     *      String signature, byte[] annotations, byte[] parameterAnnotations)
     */
   Method *constructor_constructor = get_constructor(constructor_class, "(Ljava/lang/Class;" "[" "Ljava/lang/Class;" "[" "Ljava/lang/Class;" "II" "Ljava/lang/String;" "[B[B)V");

   // invoke constructor of class java/lang/reflect/Constructor
   for (int i = 0; i < count; i++)
   {
       Method *constructor = constructors[i];
       Object *o = alloc_object(constructor_class);
       array_set_ref(constructor_array, i, o);

       exec_java(constructor_constructor, (slot_t []) {
            rslot(o),                                // this
            rslot(ofClass),                            // declaring class
            rslot(get_parameter_types(constructor)), // parameter types
            rslot(get_exception_types(constructor)), // checked exceptions
            islot(constructor->access_flags),        // modifiers todo
            islot(i),                                // slot   todo
            rslot(NULL),                            // signature  todo
            rslot(NULL),                            // annotations  todo
            rslot(NULL),                            // parameter annotations  todo
        });
    }

    free(constructors);
    return (jobjectArray) constructor_array;
}

/* Differs from JVM_GetClassModifiers in treatment of inner classes.
   This returns the access flags for the class as specified in the
   class file rather than searching the InnerClasses attribute (if
   present) to find the source-level access flags. Only the values of
   the low 13 bits (i.e., a mask of 0x1FFF) are guaranteed to be
   valid. */
JNIEXPORT jint JNICALL
JVM_GetClassAccessFlags(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassAccessFlags(env=%p, cls=%p)", env, cls);
    return JVM_MIRROR(cls)->access_flags; // todo
}

/* Nestmates - since JDK 11 */

JNIEXPORT jboolean JNICALL
JVM_AreNestMates(JNIEnv *env, jclass current, jclass member)
{
    TRACE("JVM_AreNestMates(env=%p, current=%p, member=%p)", env, current, member);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jclass JNICALL
JVM_GetNestHost(JNIEnv *env, jclass current)
{
    TRACE("JVM_GetNestHost(env=%p, current=%p)", env, current);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobjectArray JNICALL
JVM_GetNestMembers(JNIEnv *env, jclass current)
{
    TRACE("JVM_GetNestMembers(env=%p, current=%p)", env, current);
    JVM_PANIC("unimplemented"); // todo
}

/* Records - since JDK 14 */

JNIEXPORT jboolean JNICALL
JVM_IsRecord(JNIEnv *env, jclass cls)
{
    TRACE("JVM_IsRecord(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobjectArray JNICALL
JVM_GetRecordComponents(JNIEnv *env, jclass ofClass)
{
    TRACE("JVM_GetRecordComponents(env=%p, ofClass=%p)", env, ofClass);
    JVM_PANIC("unimplemented"); // todo
}

/* Sealed types - since JDK 15 */

JNIEXPORT jobjectArray JNICALL
JVM_GetPermittedSubclasses(JNIEnv *env, jclass current)
{
    TRACE("JVM_GetPermittedSubclasses(env=%p, current=%p)", env, current);
    JVM_PANIC("unimplemented"); // todo
}

/* The following two reflection routines are still needed due to startup time issues */
/*
 * java.lang.reflect.Method
 */
JNIEXPORT jobject JNICALL
JVM_InvokeMethod(JNIEnv *env, jobject _method, jobject obj, jobjectArray args0)
{
    TRACE("JVM_InvokeMethod(env=%p, method=%p, obj=%p, args0=%p)", env, _method, obj, args0);
    jref method = (jref) _method;
    jref o = (jref) obj;
    jarrRef os = (jarrRef) args0;

    // If method is static, o is NULL.

    // private Class<?>   clazz;
    // private String     name;
    // private Class<?>   returnType;
    // private Class<?>[] parameterTypes;
    Class *c = get_ref_field(method, S(clazz), S(sig_java_lang_Class))->jvm_mirror;
    jstrRef name = get_ref_field(method, S(name), S(sig_java_lang_String));
    jref rtype = get_ref_field(method, S(returnType), S(sig_java_lang_Class));
    jref ptypes = get_ref_field(method, S(parameterTypes), S(array_java_lang_Class));

    char *desc = unparse_method_descriptor(ptypes, rtype);
    Method *m = lookup_method(c, string_to_utf8(name), desc);
    if (m == NULL) {
        JVM_PANIC("error"); // todo
    }

    slot_t *result = exec_java0(m, o, os);
    switch (m->ret_type) {
    case RET_VOID:
        return (jobject) void_box();
    case RET_BYTE:
        return (jobject) byte_box(slot_get_byte(result));
    case RET_BOOL:
        return (jobject) bool_box(slot_get_bool(result));
    case RET_CHAR:
        return (jobject) char_box(slot_get_char(result));
    case RET_SHORT:
        return (jobject) short_box(slot_get_short(result));
    case RET_INT:
        return (jobject) int_box(slot_get_int(result));
    case RET_FLOAT:
        return (jobject) float_box(slot_get_float(result));
    case RET_LONG:
        return (jobject) long_box(slot_get_long(result));
    case RET_DOUBLE:
        return (jobject) double_box(slot_get_double(result));
    case RET_REFERENCE:
        return (jobject) slot_get_ref(result);
    default:
        JVM_PANIC("never go here\n"); // todo
    }
}

/*
 * java.lang.reflect.Constructor
 */
JNIEXPORT jobject JNICALL
JVM_NewInstanceFromConstructor(JNIEnv *env, jobject c, jobjectArray args0)
{
    TRACE("JVM_NewInstanceFromConstructor(env=%p, c=%p, args0=%p)", env, c, args0);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Constant pool access; currently used to implement reflective access to annotations (JDK 1.5)
 */

JNIEXPORT jobject JNICALL
JVM_GetClassConstantPool(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetClassConstantPool(env=%p, cls=%p)", env, cls);
    Class *c = load_boot_class("jdk/internal/reflect/ConstantPool");
    jref cp = alloc_object(c);
    set_ref_field(cp, "constantPoolOop", "Ljava/lang/Object;", (jref) &(((jclsRef) cls)->jvm_mirror->cp));
    return (jobject) cp;
}

JNIEXPORT jint JNICALL 
JVM_ConstantPoolGetSize(JNIEnv *env, jobject obj, jobject unused)
{
    TRACE("JVM_ConstantPoolGetSize(env=%p, obj=%p)", env, obj);
    ConstantPool *cp = (ConstantPool *) obj;
    return cp->size;
}

JNIEXPORT jclass JNICALL 
JVM_ConstantPoolGetClassAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetClassAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return (jclass) (resolve_class(cp, (u2) index)->java_mirror);
}

JNIEXPORT jclass JNICALL 
JVM_ConstantPoolGetClassAtIfLoaded(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetClassAtIfLoaded(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    // cp->type[index]
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL 
JVM_ConstantPoolGetClassRefIndexAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetClassRefIndexAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL 
JVM_ConstantPoolGetMethodAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetMethodAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL JVM_ConstantPoolGetMethodAtIfLoaded
(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetMethodAtIfLoaded(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL
JVM_ConstantPoolGetFieldAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetFieldAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobject JNICALL 
JVM_ConstantPoolGetFieldAtIfLoaded(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetFieldAtIfLoaded(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobjectArray JNICALL 
JVM_ConstantPoolGetMemberRefInfoAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetMemberRefInfoAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;

    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL 
JVM_ConstantPoolGetNameAndTypeRefIndexAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetNameAndTypeRefIndexAt(env=%p, obj=%p, index=%d)", env, obj, index);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jobjectArray JNICALL
JVM_ConstantPoolGetNameAndTypeRefInfoAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetNameAndTypeRefInfoAt(env=%p, obj=%p, index=%d)", env, obj, index);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL 
JVM_ConstantPoolGetIntAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetIntAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return cp_get_int(cp, (u2) index);
}

JNIEXPORT jlong JNICALL 
JVM_ConstantPoolGetLongAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetLongAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return cp_get_long(cp, (u2) index);
}

JNIEXPORT jfloat JNICALL 
JVM_ConstantPoolGetFloatAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetFloatAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return cp_get_float(cp, (u2) index);
}

JNIEXPORT jdouble JNICALL 
JVM_ConstantPoolGetDoubleAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetDoubleAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return cp_get_double(cp, (u2) index);
}

JNIEXPORT jstring JNICALL 
JVM_ConstantPoolGetStringAt(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetStringAt(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    return (jstring) resolve_string(cp, index);
}

JNIEXPORT jstring JNICALL 
JVM_ConstantPoolGetUTF8At(JNIEnv *env, jobject obj, jobject unused, jint index)
{
    TRACE("JVM_ConstantPoolGetUTF8At(env=%p, obj=%p, index=%d)", env, obj, index);
    ConstantPool *cp = (ConstantPool *) obj;
    utf8_t *utf8 = cp_utf8(cp, index);
    return (jstring) alloc_string(utf8);
}

JNIEXPORT jbyte JNICALL 
JVM_ConstantPoolGetTagAt(JNIEnv *env, jobject unused, jobject jcpool, jint index)
{
    TRACE("JVM_ConstantPoolGetTagAt()"); // todo
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Parameter reflection
 */

JNIEXPORT jobjectArray JNICALL
JVM_GetMethodParameters(JNIEnv *env, jobject method)
{
    TRACE("JVM_GetMethodParameters(env=%p, method=%p)", env, method);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.security.*
 */

// JNIEXPORT jobject JNICALL
// JVM_DoPrivileged(JNIEnv *env, jclass cls, jobject action, jobject context, jboolean wrapException)
// {
//     TRACE("JVM_DoPrivileged(env=%p, cls=%p, action=%p, context=%p, wrapException=%d)", 
//                             env, cls, action, context, wrapException);
//     JVM_PANIC("unimplemented"); // todo
// }

JNIEXPORT jobject JNICALL
JVM_GetInheritedAccessControlContext(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetInheritedAccessControlContext(env=%p, cls=%p)", env, cls);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Ensure that code doing a stackwalk and using javaVFrame::locals() to
 * get the value will see a materialized value and not a scalar-replaced
 * null value.
 */
#define JVM_EnsureMaterializedForStackWalk(env, value) \
    do {} while(0) // Nothing to do.  The fact that the value escaped
                   // through a native method is enough.

JNIEXPORT jobject JNICALL
JVM_GetStackAccessControlContext(JNIEnv *env, jclass cls)
{
    TRACE("JVM_GetStackAccessControlContext(env=%p, cls=%p)", env, cls);
    // JVM_PANIC("unimplemented"); // todo
    return NULL; // todo
}

/*
 * Signal support, used to implement the shutdown sequence.  Every VM must
 * support JVM_SIGINT and JVM_SIGTERM, raising the former for user interrupts
 * (^C) and the latter for external termination (kill, system shutdown, etc.).
 * Other platform-dependent signal values may also be supported.
 */

JNIEXPORT void * JNICALL
JVM_RegisterSignal(jint sig, void *handler)
{
    TRACE("JVM_RegisterSignal(sig=%d, handler=%p)", sig, handler);

    // todo
    return handler;

    // JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jboolean JNICALL
JVM_RaiseSignal(jint sig)
{
    TRACE("JVM_RaiseSignal(sig=%d)", sig);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL
JVM_FindSignal(const char *name)
{
    TRACE("JVM_FindSignal(name=%s)", name);
    // JVM_PANIC("unimplemented"); // todo
    return 0; // todo
}

/*
 * Retrieve the assertion directives for the specified class.
 */
JNIEXPORT jboolean JNICALL
JVM_DesiredAssertionStatus(JNIEnv *env, jclass unused, jclass cls)
{
    TRACE("JVM_DesiredAssertionStatus(env=%p, cls=%p)", env, cls);

    // JVM_PANIC("unimplemented"); // todo

    // todo 本vm不讨论断言。desiredAssertionStatus0（）方法把false推入操作数栈顶
    return false;
}

/*
 * Retrieve the assertion directives from the VM.
 */
JNIEXPORT jobject JNICALL
JVM_AssertionStatusDirectives(JNIEnv *env, jclass unused)
{
    TRACE("JVM_AssertionStatusDirectives(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.util.concurrent.atomic.AtomicLong
 */
JNIEXPORT jboolean JNICALL
JVM_SupportsCX8(void)
{
    TRACE("JVM_SupportsCX8()");
    JVM_PANIC("unimplemented"); // todo
}

// JNIEXPORT jboolean JNICALL
// JVM_CX8Field(JNIEnv *env, jobject obj, jfieldID fldID, jlong oldVal, jlong newVal)
// {
//     TRACE("JVM_CX8Field(env=%p, obj=%p, fldID=%p, oldVal=%lld, newVal=%lld)", 
//                         env, obj, fldID, oldVal, newVal);
//     JVM_PANIC("unimplemented"); // todo
// }

/*
 * com.sun.dtrace.jsdt support
 */

#define JVM_TRACING_DTRACE_VERSION 1

/*
 * Structure to pass one probe description to JVM.
 *
 * The VM will overwrite the definition of the referenced method with
 * code that will fire the probe.
 */
typedef struct {
    jmethodID method;
    jstring   function;
    jstring   name;
    void*     reserved[4];     // for future use
} JVM_DTraceProbe;

/**
 * Encapsulates the stability ratings for a DTrace provider field
 */
typedef struct {
    jint nameStability;
    jint dataStability;
    jint dependencyClass;
} JVM_DTraceInterfaceAttributes;

/*
 * Structure to pass one provider description to JVM
 */
typedef struct {
    jstring                       name;
    JVM_DTraceProbe*              probes;
    jint                          probe_count;
    JVM_DTraceInterfaceAttributes providerAttributes;
    JVM_DTraceInterfaceAttributes moduleAttributes;
    JVM_DTraceInterfaceAttributes functionAttributes;
    JVM_DTraceInterfaceAttributes nameAttributes;
    JVM_DTraceInterfaceAttributes argsAttributes;
    void*                         reserved[4]; // for future use
} JVM_DTraceProvider;

/*
 * Get the version number the JVM was built with
 */
JNIEXPORT jint JNICALL
JVM_DTraceGetVersion(JNIEnv* env)
{
    TRACE("JVM_DTraceGetVersion(env=%p", env);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Register new probe with given signature, return global handle
 *
 * The version passed in is the version that the library code was
 * built with.
 */
JNIEXPORT jlong JNICALL
JVM_DTraceActivate(JNIEnv* env, jint version, jstring moduleName,
                    jint providersCount, JVM_DTraceProvider* providers)
{
    TRACE("JVM_DTraceActivate(env=%p, version=%d, moduleName=%p, providersCount=%d, providers=%p)",
                        env, version, moduleName, providersCount, providers);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Check JSDT probe
 */
JNIEXPORT jboolean JNICALL
JVM_DTraceIsProbeEnabled(JNIEnv* env, jmethodID method)
{
    TRACE("JVM_DTraceIsProbeEnabled(env=%p, method=%p", env, method);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Destroy custom DOF
 */
JNIEXPORT void JNICALL
JVM_DTraceDispose(JNIEnv* env, jlong handle)
{
    TRACE("JVM_DTraceDispose(env=%p, handle=%lld", env, handle);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Check to see if DTrace is supported by OS
 */
JNIEXPORT jboolean JNICALL
JVM_DTraceIsSupported(JNIEnv* env)
{
    TRACE("JVM_DTraceIsSupported(env=%p", env);
    JVM_PANIC("unimplemented"); // todo
}

/*************************************************************************
 PART 2: Support for the Verifier and Class File Format Checker
 ************************************************************************/
/*
 * Return the class name in UTF format. The result is valid
 * until JVM_ReleaseUTf is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetClassNameUTF(JNIEnv *env, jclass cb)
{
    TRACE("JVM_GetClassNameUTF(env=%p, cb=%p", env, cb);
    JVM_PANIC("unimplemented"); // todo
    // Class *c = JVM_MIRROR(cb);
    // return c->class_name;
}

// Returns the constant pool types in the buffer provided by "types."
JNIEXPORT void JNICALL
JVM_GetClassCPTypes(JNIEnv *env, jclass cb, unsigned char *types)
{
    TRACE("JVM_GetClassCPTypes(env=%p, cb=%p, types=%p", env, cb, types);
    JVM_PANIC("unimplemented"); // todo
}

// Returns the number of Constant Pool entries.
JNIEXPORT jint JNICALL
JVM_GetClassCPEntriesCount(JNIEnv *env, jclass cb)
{
    TRACE("JVM_GetClassCPEntriesCount(env=%p, cb=%p", env, cb);
    Class *c = JVM_MIRROR(cb);
    return c->cp.size;
}

// Returns the number of *declared* fields or methods.
JNIEXPORT jint JNICALL
JVM_GetClassFieldsCount(JNIEnv *env, jclass cb)
{
    TRACE("JVM_GetClassFieldsCount(env=%p, cb=%p", env, cb);
    Class *c = JVM_MIRROR(cb);
    return c->fields_count;
}

JNIEXPORT jint JNICALL
JVM_GetClassMethodsCount(JNIEnv *env, jclass cb)
{
    TRACE("JVM_GetClassMethodsCount(env=%p, cb=%p", env, cb);
     Class *c = JVM_MIRROR(cb);
    return c->methods_count;
}

/*
 * Returns the CP indexes of exceptions raised by a given method.
 * Places the result in the given buffer.
 *
 * The method is identified by method_index.
 */
JNIEXPORT void JNICALL
JVM_GetMethodIxExceptionIndexes(JNIEnv *env, jclass cb, jint method_index, unsigned short *exceptions)
{
    TRACE("JVM_GetMethodIxExceptionIndexes(env=%p, cb=%p, method_index=%d, exceptions=%p", 
                                            env, cb, method_index, exceptions);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the number of exceptions raised by a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxExceptionsCount(JNIEnv *env, jclass cb, jint method_index)
{
    TRACE("JVM_GetMethodIxExceptionsCount(env=%p, cb=%p, method_index=%d)",
                                            env, cb, method_index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the byte code sequence of a given method.
 * Places the result in the given buffer.
 *
 * The method is identified by method_index.
 */
JNIEXPORT void JNICALL
JVM_GetMethodIxByteCode(JNIEnv *env, jclass cb, jint method_index, unsigned char *code)
{
    TRACE("JVM_GetMethodIxByteCode(env=%p, cb=%p, method_index=%d, code=%p", 
                                            env, cb, method_index, code);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the length of the byte code sequence of a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxByteCodeLength(JNIEnv *env, jclass cb, jint method_index)
{
    TRACE("JVM_GetMethodIxByteCodeLength(env=%p, cb=%p, method_index=%d)",
                                            env, cb, method_index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * A structure used to a capture exception table entry in a Java method.
 */
typedef struct {
    jint start_pc;
    jint end_pc;
    jint handler_pc;
    jint catchType;
} JVM_ExceptionTableEntryType;

/*
 * Returns the exception table entry at entry_index of a given method.
 * Places the result in the given buffer.
 *
 * The method is identified by method_index.
 */
JNIEXPORT void JNICALL
JVM_GetMethodIxExceptionTableEntry(JNIEnv *env, jclass cb, jint method_index,
                                   jint entry_index, JVM_ExceptionTableEntryType *entry)
{
    TRACE("JVM_GetMethodIxExceptionTableEntry(env=%p, cb=%p, method_index=%d, entry_index=%d, entry=%p)",
                                            env, cb, method_index, entry_index, entry);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the length of the exception table of a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxExceptionTableLength(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetMethodIxExceptionTableLength(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the modifiers of a given field.
 * The field is identified by field_index.
 */
JNIEXPORT jint JNICALL
JVM_GetFieldIxModifiers(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetFieldIxModifiers(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the modifiers of a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxModifiers(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetMethodIxModifiers(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the number of local variables of a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxLocalsCount(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetMethodIxLocalsCount(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the number of arguments (including this pointer) of a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxArgsSize(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetMethodIxArgsSize(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the maximum amount of stack (in words) used by a given method.
 * The method is identified by method_index.
 */
JNIEXPORT jint JNICALL
JVM_GetMethodIxMaxStack(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_GetMethodIxMaxStack(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Is a given method a constructor.
 * The method is identified by method_index.
 */
JNIEXPORT jboolean JNICALL
JVM_IsConstructorIx(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_IsConstructorIx(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Is the given method generated by the VM.
 * The method is identified by method_index.
 */
JNIEXPORT jboolean JNICALL
JVM_IsVMGeneratedMethodIx(JNIEnv *env, jclass cb, int index)
{
    TRACE("JVM_IsVMGeneratedMethodIx(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the name of a given method in UTF format.
 * The result remains valid until JVM_ReleaseUTF is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetMethodIxNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetMethodIxNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the signature of a given method in UTF format.
 * The result remains valid until JVM_ReleaseUTF is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetMethodIxSignatureUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetMethodIxSignatureUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the name of the field refered to at a given constant pool
 * index.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPFieldNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPFieldNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the name of the method refered to at a given constant pool
 * index.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPMethodNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPMethodNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the signature of the method refered to at a given constant pool
 * index.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPMethodSignatureUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPMethodSignatureUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the signature of the field refered to at a given constant pool
 * index.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPFieldSignatureUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPFieldSignatureUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the class name refered to at a given constant pool index.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPClassNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPClassNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the class name refered to at a given constant pool index.
 *
 * The constant pool entry must refer to a CONSTANT_Fieldref.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPFieldClassNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPFieldClassNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the class name refered to at a given constant pool index.
 *
 * The constant pool entry must refer to CONSTANT_Methodref or
 * CONSTANT_InterfaceMethodref.
 *
 * The result is in UTF format and remains valid until JVM_ReleaseUTF
 * is called.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 */
JNIEXPORT const char * JNICALL
JVM_GetCPMethodClassNameUTF(JNIEnv *env, jclass cb, jint index)
{
    TRACE("JVM_GetCPMethodClassNameUTF(env=%p, cb=%p, index=%d)", env, cb, index);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the modifiers of a field in calledClass. The field is
 * referred to in class cb at constant pool entry index.
 *
 * The caller must treat the string as a constant and not modify it
 * in any way.
 *
 * Returns -1 if the field does not exist in calledClass.
 */
JNIEXPORT jint JNICALL
JVM_GetCPFieldModifiers(JNIEnv *env, jclass cb, int index, jclass calledClass)
{
    TRACE("JVM_GetCPFieldModifiers(env=%p, cb=%p, index=%d, calledClass=%p)", 
                                    env, cb, index, calledClass);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * Returns the modifiers of a method in calledClass. The method is
 * referred to in class cb at constant pool entry index.
 *
 * Returns -1 if the method does not exist in calledClass.
 */
JNIEXPORT jint JNICALL
JVM_GetCPMethodModifiers(JNIEnv *env, jclass cb, int index, jclass calledClass)
{
    TRACE("JVM_GetCPMethodModifiers(env=%p, cb=%p, index=%d, calledClass=%p)",
                                    env, cb, index, calledClass);
    JVM_PANIC("unimplemented"); // todo
}

// Releases the UTF string obtained from the VM.
JNIEXPORT void JNICALL
JVM_ReleaseUTF(const char *utf)
{
    TRACE("JVM_ReleaseUTF(utf=%s)", utf);
    JVM_PANIC("unimplemented"); // todo
}

// Compare if two classes are in the same package.
JNIEXPORT jboolean JNICALL
JVM_IsSameClassPackage(JNIEnv *env, jclass class1, jclass class2)
{
    TRACE("JVM_IsSameClassPackage(env=%p, class1=%p, class2=%p)", env, class1, class2);
    return utf8_equals(JVM_MIRROR(class1)->pkg_name, JVM_MIRROR(class2)->pkg_name);
}


// /*
//  * A function defined by the byte-code verifier and called by the VM.
//  * This is not a function implemented in the VM.
//  *
//  * Returns JNI_FALSE if verification fails. A detailed error message
//  * will be places in msg_buf, whose length is specified by buf_len.
//  */
// typedef jboolean (*verifier_fn_t)(JNIEnv *env,
//                                   jclass cb,
//                                   char * msg_buf,
//                                   jint buf_len);


/*
 * Support for a VM-independent class format checker.
 */
typedef struct {
    unsigned long code;    /* byte code */
    unsigned long excs;    /* exceptions */
    unsigned long etab;    /* catch table */
    unsigned long lnum;    /* line number */
    unsigned long lvar;    /* local vars */
} method_size_info;

typedef struct {
    unsigned int constants;    /* constant pool */
    unsigned int fields;
    unsigned int methods;
    unsigned int interfaces;
    unsigned int fields2;      /* number of static 2-word fields */
    unsigned int innerclasses; /* # of records in InnerClasses attr */

    method_size_info clinit;   /* memory used in clinit */
    method_size_info main;     /* used everywhere else */
} class_size_info;

// /*
//  * Functions defined in libjava.so to perform string conversions.
//  *
//  */

// typedef jstring (*to_java_string_fn_t)(JNIEnv *env, char *str);

// typedef char *(*to_c_string_fn_t)(JNIEnv *env, jstring s, jboolean *b);

// /* This is the function defined in libjava.so that performs class
//  * format checks. This functions fills in size information about
//  * the class file and returns:
//  *
//  *   0: good
//  *  -1: out of memory
//  *  -2: bad format
//  *  -3: unsupported version
//  *  -4: bad class name
//  */

// typedef jint (*check_format_fn_t)(char *class_name,
//                                   unsigned char *data,
//                                   unsigned int data_size,
//                                   class_size_info *class_size,
//                                   char *message_buffer,
//                                   jint buffer_length,
//                                   jboolean measure_only,
//                                   jboolean check_relaxed);

#define JVM_RECOGNIZED_CLASS_MODIFIERS (JVM_ACC_PUBLIC | \
                                        JVM_ACC_FINAL | \
                                        JVM_ACC_SUPER | \
                                        JVM_ACC_INTERFACE | \
                                        JVM_ACC_ABSTRACT | \
                                        JVM_ACC_ANNOTATION | \
                                        JVM_ACC_ENUM | \
                                        JVM_ACC_SYNTHETIC)

#define JVM_RECOGNIZED_FIELD_MODIFIERS (JVM_ACC_PUBLIC | \
                                        JVM_ACC_PRIVATE | \
                                        JVM_ACC_PROTECTED | \
                                        JVM_ACC_STATIC | \
                                        JVM_ACC_FINAL | \
                                        JVM_ACC_VOLATILE | \
                                        JVM_ACC_TRANSIENT | \
                                        JVM_ACC_ENUM | \
                                        JVM_ACC_SYNTHETIC)

#define JVM_RECOGNIZED_METHOD_MODIFIERS (JVM_ACC_PUBLIC | \
                                         JVM_ACC_PRIVATE | \
                                         JVM_ACC_PROTECTED | \
                                         JVM_ACC_STATIC | \
                                         JVM_ACC_FINAL | \
                                         JVM_ACC_SYNCHRONIZED | \
                                         JVM_ACC_BRIDGE | \
                                         JVM_ACC_VARARGS | \
                                         JVM_ACC_NATIVE | \
                                         JVM_ACC_ABSTRACT | \
                                         JVM_ACC_STRICT | \
                                         JVM_ACC_SYNTHETIC)


/*************************************************************************
 PART 3: I/O and Network Support
 ************************************************************************/

/*
 * Convert a pathname into native format.  This function does syntactic
 * cleanup, such as removing redundant separator characters.  It modifies
 * the given pathname string in place.
 */
JNIEXPORT char * JNICALL
JVM_NativePath(char *p)
{
    TRACE("JVM_NativePath(p=%p)", p);
    JVM_PANIC("unimplemented"); // todo
}

/*
 * The standard printing functions supported by the Java VM. (Should they
 * be renamed to JVM_* in the future?
 */

/* jio_snprintf() and jio_vsnprintf() behave like snprintf(3) and vsnprintf(3),
 *  respectively, with the following differences:
 * - The string written to str is always zero-terminated, also in case of
 *   truncation (count is too small to hold the result string), unless count
 *   is 0. In case of truncation count-1 characters are written and '\0'
 *   appendend.
 * - If count is too small to hold the whole string, -1 is returned across
 *   all platforms. */

JNIEXPORT int
jio_vsnprintf(char *str, size_t count, const char *fmt, va_list args)
{
    TRACE("jio_vsnprintf()"); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT int
jio_snprintf(char *str, size_t count, const char *fmt, ...)
{
    TRACE("jio_snprintf()"); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT int
jio_fprintf(FILE *f, const char *fmt, ...)
{
    TRACE("jio_fprintf()"); // todo
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT int
jio_vfprintf(FILE *f, const char *fmt, va_list args)
{
    TRACE("jio_vfprintf()"); // todo
    JVM_PANIC("unimplemented"); // todo
}


JNIEXPORT void * JNICALL
JVM_RawMonitorCreate(void)
{
    TRACE("JVM_RawMonitorCreate()");  
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_RawMonitorDestroy(void *mon)
{
    TRACE("JVM_RawMonitorDestroy(mon=%p)", mon);  
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jint JNICALL
JVM_RawMonitorEnter(void *mon)
{
    TRACE("JVM_RawMonitorEnter(mon=%p)", mon);  
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT void JNICALL
JVM_RawMonitorExit(void *mon)
{
    TRACE("JVM_RawMonitorExit(mon=%p)", mon);  
    JVM_PANIC("unimplemented"); // todo
}

/*
 * java.lang.management support
 */
JNIEXPORT void* JNICALL
JVM_GetManagement(jint version)
{
    TRACE("JVM_GetManagement(version=%d)", version);  
    JVM_PANIC("unimplemented"); // todo
}

/*
 * com.sun.tools.attach.VirtualMachine support
 *
 * Initialize the agent properties with the properties maintained in the VM.
 */
JNIEXPORT jobject JNICALL
JVM_InitAgentProperties(JNIEnv *env, jobject agent_props)
{
    TRACE("JVM_InitAgentProperties(env=%p, agent_props=%p)", env, agent_props);  
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jstring JNICALL
JVM_GetTemporaryDirectory(JNIEnv *env)
{
    TRACE("JVM_GetTemporaryDirectory(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}

JNIEXPORT jstring JNICALL
JVM_GetThreadInterruptEvent(JNIEnv *env)
{// ------- todo 参数和返回值都不对 --------------------------------------------------------------------------------------------
    TRACE("JVM_GetThreadInterruptEvent(env=%p)", env);
    JVM_PANIC("unimplemented"); // todo
}


/* Generics reflection support.
 *
 * Returns information about the given class's EnclosingMethod
 * attribute, if present, or null if the class had no enclosing
 * method.
 *
 * If non-null, the returned array contains three elements. Element 0
 * is the java.lang.Class of which the enclosing method is a member,
 * and elements 1 and 2 are the java.lang.Strings for the enclosing
 * method's name and descriptor, respectively.
 */
JNIEXPORT jobjectArray JNICALL
JVM_GetEnclosingMethodInfo(JNIEnv* env, jclass ofClass)
{
    TRACE("JVM_GetEnclosingMethodInfo(env=%p, ofClass=%p)", env, ofClass);

    jclsRef c = (jclsRef) ofClass;
    JVM_PANIC("unimplemented"); // todo
}