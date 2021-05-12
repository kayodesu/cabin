#include <assert.h>
#include "cabin.h"
#include "jni.h"
#include "jvm.h"
#include "util/hash.h"
#include "util/encoding.h"
#include "util/convert.h"
#include "util/endianness.h"
#include "native/jni_internal.h"
#include "objects/object.h"
#include "runtime/frame.h"
#include "runtime/vm_thread.h"
#include "interpreter/interpreter.h"

// public final native Class<?> getClass();
static jobject getClass(JNIEnv *env, jobject this)
{
    if (this == NULL) {
        // JNU_ThrowNullPointerException(env, NULL); todo
        return NULL;
    } else {
        return (*env)->GetObjectClass(env, this);
    }
}

static JNINativeMethod java_lang_Object_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "hashCode", "()I", JVM_IHashCode },
        { "getClass", __CLS, getClass },
        { "clone", __OBJ, JVM_Clone },
        { "notifyAll", "()V", JVM_MonitorNotifyAll },
        { "notify", "()V", JVM_MonitorNotify },
        { "wait", "(J)V", JVM_MonitorWait },
        { NULL }
};

//  static native Class<?> defineClass0(ClassLoader loader, Class<?> lookup, String name,
//                                        byte[] b, int off, int len,
//                                        ProtectionDomain pd, boolean initialize,
//                                        int flags, Object classData);
static jclass defineClass0(JNIEnv *env, jclass cls, jobject loader, jclsRef lookup, jstrRef name, jref b,
                             jint off, jint len, jobject pd, jbool initialize, jint flags, jref classData)
{
    const jbyte *data = ((jbyte *) b->data) + off;
    return JVM_DefineClass(env, string_to_utf8(name), loader, data, len, pd);
}

// static native Class<?> defineClass1(ClassLoader loader, String name, byte[] b, int off, int len,
//                                        ProtectionDomain pd, String source);
static jclass defineClass1(JNIEnv *env, jclass cls, jobject loader, jstrref name,
                                   jref b, jint off, jint len, jobject pd, jstrRef source)
{
    const jbyte *data = ((jbyte *) b->data) + off;
    return JVM_DefineClassWithSource(env, string_to_utf8(name), loader, data, len, pd, string_to_utf8(source));
}

// static native Class<?> defineClass2(ClassLoader loader, String name, java.nio.ByteBuffer b,
//                                        int off, int len, ProtectionDomain pd, String source);
static jclsref defineClass2(JNIEnv *env, jclass cls, jref loader,
                                   jstrref name, jref b, jint off, jint len, jref pd, jstrref source)
{
    // todo
    JVM_PANIC("defineClass2");
}

// load bootstrap class
// private native Class<?> findBootstrapClass(String name);
static jclass findBootstrapClass(JNIEnv *env, jref this, jstrref name)
{
    utf8_t *slash_name = dot_to_slash_dup(string_to_utf8(name));
    return JVM_FindClassFromBootLoader(env, slash_name);
}

// private static native String findBuiltinLib(String name);
static jstrref findBuiltinLib(JNIEnv *env, jclass cls, jstrref name)
{
    const char *utf8_name = string_to_utf8(name);
    if (utf8_equals(utf8_name, "zip.dll")) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = "(C:\\Progles (x86)\\Java\\jre1.8.0_221\\bin\\zip.dll)";  // todo
        return alloc_string(buf); // todo
    } else if (utf8_equals(utf8_name, "management.dll")) {
        return alloc_string("ffffffff"); // todo
    } else {
        JVM_PANIC(utf8_name); // todo
    }
}

// Retrieves the assertion directives from the VM.
// private static native AssertionStatusDirectives retrieveDirectives();
static void retrieveDirectives(JNIEnv *env, jclass cls)
{
    // todo
    JVM_PANIC("retrieveDirectives");
}

#undef PD
#define PD "Ljava/security/ProtectionDomain;"

static JNINativeMethod java_lang_ClassLoader_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "defineClass0", "(Ljava/lang/ClassLoader;" STR "[BII" PD "ZI" OBJ_ CLS, defineClass0 },
        { "defineClass1", "(Ljava/lang/ClassLoader;" STR "[BII" PD STR ")" CLS, defineClass1 },
        { "defineClass2", "(Ljava/lang/ClassLoader;" STR "Ljava/nio/ByteBuffer;II" PD STR ")" CLS, defineClass2 },
        { "findBootstrapClass", _STR_ CLS, findBootstrapClass },
        { "findLoadedClass0", _STR_ CLS, JVM_FindLoadedClass },
        { "findBuiltinLib", _STR_ STR, findBuiltinLib },
        { NULL }
};

#undef PD


/**
 * Maps a library name into a platform-specific string representing a native library.
 *
 * @param      libname the name of the library.
 * @return     a platform-dependent native library name.
 * @exception  NullPointerException if <code>libname</code> is
 *             <code>null</code>
 * @see        java.lang.System#loadLibrary(java.lang.String)
 * @see        java.lang.ClassLoader#findLibrary(java.lang.String)
 * @since      1.2
 */
// public static native String mapLibraryName(String libname);
static jstrref mapLibraryName(JNIEnv *env, jclass cls, jstrref libname)
{
    if (libname == NULL) {
        // throw java_lang_NullPointerException();
        raise_exception(S(java_lang_NullPointerException), NULL);
    }

    const char *name = string_to_utf8(libname);
    char mapping_name[strlen(name) + 5];;
    strcpy(mapping_name, name);
    strcat(mapping_name, ".dll"); // todo ...........................
    return alloc_string(mapping_name);  // todo
}

// public static native int identityHashCode(Object x);
static jint identityHashCode(JNIEnv *env, jclass cls, jref x)
{
    return (jint) (intptr_t) x; // todo 实现错误。改成当前的时间如何。
}

// private static native Properties initProperties(Properties props);
// static jref initProperties(JNIEnv * env, jclass cls, jref props)
// {
//     JVM_InitProperties(env, props);
// }

// private static native void setIn0(InputStream in);
static void setIn0(JNIEnv *env, jclass cls, jref in)
{
    Class *clazz = get_current_thread()->top_frame->method->clazz;
    lookup_static_field(clazz, "in", "Ljava/io/InputStream;")->static_value.r = in;
}

// private static native void setOut0(PrintStream out);
static void setOut0(JNIEnv *env, jclass cls, jref out)
{
    Class *clazz = get_current_thread()->top_frame->method->clazz;
    lookup_static_field(clazz, "out", "Ljava/io/PrintStream;")->static_value.r = out;
}

// private static native void setErr0(PrintStream err);
static void setErr0(JNIEnv *env, jclass cls, jref err)
{
    Class *clazz = get_current_thread()->top_frame->method->clazz;
    lookup_static_field(clazz, "err", "Ljava/io/PrintStream;")->static_value.r = err;
}

static JNINativeMethod java_lang_System_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "mapLibraryName", _STR_ STR, mapLibraryName },
        { "arraycopy", _OBJ "ILjava/lang/Object;II)V", JVM_ArrayCopy },
        { "identityHashCode", _OBJ_ "I", identityHashCode },
        // { "initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties },

        { "setIn0", "(Ljava/io/InputStream;)V", setIn0 },
        { "setOut0", "(Ljava/io/PrintStream;)V", setOut0 },
        { "setErr0", "(Ljava/io/PrintStream;)V", setErr0 },

        { "nanoTime", "()J", JVM_NanoTime },
        { "currentTimeMillis", "()J", JVM_CurrentTimeMillis },
        { NULL }
};

// public static native int floatToRawIntBits(float value);
static jint floatToRawIntBits(JNIEnv *env, jclass cls, jfloat value)
{
    return float_to_raw_int_bits(value);
}

// public static native float intBitsToFloat(int value);
static jfloat intBitsToFloat(JNIEnv *env, jclass cls, jint value)
{
    return int_bits_to_float(value);
}

static JNINativeMethod java_lang_Float_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "floatToRawIntBits", "(F)I", floatToRawIntBits },
        { "intBitsToFloat", "(I)F", intBitsToFloat },
        { NULL }
};

// public static native long doubleToRawLongBits(double value);
static jlong doubleToRawLongBits(JNIEnv *env, jclass cls, jdouble value)
{
    return double_to_raw_long_bits(value);
}

// public static native double longBitsToDouble(long value);
static jdouble longBitsToDouble(JNIEnv *env, jclass cls, jlong value)
{
    return long_bits_to_double(value);
}

static JNINativeMethod java_lang_Double_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "doubleToRawLongBits", "(D)J", doubleToRawLongBits },
        { "longBitsToDouble", "(J)D", longBitsToDouble },
        { NULL }
};

static JNINativeMethod java_lang_String_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "intern", "()Ljava/lang/String;", JVM_InternString },
        { NULL }
};

static jstring getSystemPackage0(JNIEnv *env, jclass cls, jstring name)
{
    return JVM_GetSystemPackage(env, name);
}

// private static native String[] getSystemPackages0();
static jobjectArray getSystemPackages0(JNIEnv *env, jclass cls)
{
    return JVM_GetSystemPackages(env);
}

static JNINativeMethod java_lang_Package_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "getSystemPackage0", _STR_ STR, getSystemPackage0 },
        { "getSystemPackages0", "()[" STR, getSystemPackages0 },
        { NULL }
};

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
// private static native void clearInterruptEvent();
static void clearInterruptEvent(JNIEnv *env, jclass cls)
{
    JVM_PANIC("clearInterruptEvent"); // todo
}

static JNINativeMethod java_lang_Thread_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "currentThread", "()Ljava/lang/Thread;", JVM_CurrentThread },
        { "yield", "()V", JVM_Yield },
        { "sleep", "(J)V", JVM_Sleep },
        { "interrupt0", "()V", JVM_Interrupt },
        // { "isInterrupted", "(Z)Z", JVM_IsInterrupted },
        { "isAlive", "()Z", JVM_IsThreadAlive },
        { "start0", "()V", JVM_StartThread },
        // { "countStackFrames", "()I", JVM_CountStackFrames },
        { "holdsLock", "(Ljava/lang/Object;)Z", JVM_HoldsLock },
        { "dumpThreads", "([Ljava/lang/Thread;)[[Ljava/lang/StackTraceElement;", JVM_DumpThreads },
        { "getThreads", "()[Ljava/lang/Thread;", JVM_GetAllThreads },
        { "setPriority0", "(I)V", JVM_SetThreadPriority },
        { "stop0", "(Ljava/lang/Object;)V", JVM_StopThread },
        { "suspend0", "()V", JVM_SuspendThread },
        { "resume0", "()V", JVM_ResumeThread },
        { "clearInterruptEvent", "()V", clearInterruptEvent },
        { "setNativeName", "(Ljava/lang/String;)V", JVM_SetNativeThreadName },
        { NULL }
};


// private native Throwable fillInStackTrace(int dummy);
static jobject fillInStackTrace(JNIEnv *env, jobject throwable, jint dummy)
{
    JVM_FillInStackTrace(env, throwable);
    return throwable;
}

static JNINativeMethod java_lang_Throwable_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace },
        { NULL }
};

// private static native boolean isBigEndian();
static jbool isBigEndian(JNIEnv *env, jclass cls)
{
    return is_big_endian() ? true : false;
}

static JNINativeMethod java_lang_StringUTF16_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "isBigEndian", "()Z", isBigEndian },
        { NULL }
};

static JNINativeMethod java_lang_NullPointerException_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "getExtendedNPEMessage", __STR, JVM_GetExtendedNPEMessage },
        { NULL }
};

/*
 * Sets the given stack trace elements with the backtrace of the given Throwable.
 *
 * private static native void initStackTraceElements(StackTraceElement[] elements, Throwable x);
 */
static void initStackTraceElements(JNIEnv *env, jclass cls, jobject elements, jobject x)
{
    JVM_InitStackTraceElementArray(env, elements, x);
}

/*
 * Sets the given stack trace element with the given StackFrameInfo
 *
 * private static native void initStackTraceElement(StackTraceElement element, StackFrameInfo sfi);
 */
static void initStackTraceElement(JNIEnv *env, jclass cls, jobject elements, jobject sfi)
{
    JVM_InitStackTraceElement(env, elements, sfi);
}

static JNINativeMethod java_lang_StackTraceElement_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "initStackTraceElements", "([Ljava/lang/StackTraceElement;Ljava/lang/Throwable;)V", initStackTraceElements },
        { "initStackTraceElement", "(Ljava/lang/StackTraceElement;Ljava/lang/StackFrameInfo;)V", initStackTraceElement },
        { NULL }
};

// public native int availableProcessors();
static jint availableProcessors(JNIEnv *env, jref this)
{
    return JVM_ActiveProcessorCount();
}

// public native long freeMemory();
static jlong freeMemory(JNIEnv *env, jref this)
{
    return JVM_FreeMemory();
}

// public native long totalMemory();
static jlong totalMemory(JNIEnv *env, jref this)
{
    return JVM_TotalMemory();
}

// public native long maxMemory();
static jlong maxMemory(JNIEnv *env, jref this)
{
    return JVM_MaxMemory();
}

// public native void gc();
static void gc(JNIEnv *env, jref this)
{
    JVM_GC();
}

static JNINativeMethod java_lang_Runtime_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "availableProcessors", "()I", availableProcessors },
        { "freeMemory", "()J", freeMemory },
        { "totalMemory", "()J", totalMemory },
        { "maxMemory", "()J", maxMemory },
        { "gc", "()V", gc },
        { NULL }
};

// private static native void addExports0(Module from, String pn, Module to);
static void addExports0(JNIEnv *env, jclass cls, jobject from, jstring pn, jobject to)
{
    JVM_AddModuleExports(env, from, pn, to);
}

// private static native void addExportsToAll0(Module from, String pn);
static void addExportsToAll0(JNIEnv *env, jclass cls, jobject from, jstring pn)
{
    JVM_AddModuleExportsToAll(env, from, pn);
}

// private static native void addExportsToAllUnnamed0(Module from, String pn);
static void addExportsToAllUnnamed0(JNIEnv *env, jclass cls, jobject from, jstring pn)
{
    JVM_AddModuleExportsToAllUnnamed(env, from, pn);
}

#undef _M
#define _M "(Ljava/lang/Module;"

static JNINativeMethod java_lang_Module_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "defineModule0", _M "Z" STR STR "[" STR_ "V", JVM_DefineModule },
        { "addReads0", _M "Ljava/lang/Module;)V", JVM_AddReadsModule },
        { "addExports0", _M STR "Ljava/lang/Module;)V", addExports0 },
        { "addExportsToAll0", _M STR_ "V", addExportsToAll0 },
        { "addExportsToAllUnnamed0", _M STR_ "V", addExportsToAllUnnamed0 },
        { NULL }
};
#undef _M

static void beforeHalt(JNIEnv *env, jclass cls)
{
    JVM_BeforeHalt();
}

static void halt0(JNIEnv *env, jclass cls, jint status)
{
    JVM_Halt(status);
}

static JNINativeMethod java_lang_Shutdown_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "beforeHalt", "()V", beforeHalt },
        { "halt0", "(I)V", halt0 },
        { NULL }
};

/*
 * private static native Object newInstance0(Constructor<?> c, Object[] args)
 * throws InstantiationException, IllegalArgumentException, InvocationTargetException;
 */
static jobject newInstanceFromConstructor(JNIEnv *env, jclass cls, jobject c, jobjectArray args)
{
    return JVM_NewInstanceFromConstructor(env, c, args);
}

static JNINativeMethod sun_reflect_NativeConstructorAccessorImpl_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "newInstance0", "(Ljava/lang/reflect/Constructor;[" OBJ_ OBJ, newInstanceFromConstructor },
        { NULL }
};

// // public static native Class<?> getCallerClass(int level)
// static jclsref getCallerClass0(JNIEnv *env, jclass cls, jint level)
// {
//     return JVM_GetCallerClass(env, level);
// }

// public static native Class<?> getCallerClass()
static jclass getCallerClass(JNIEnv *env, jclass cls)
{
    return JVM_GetCallerClass(env);
}

// public static native int getClassAccessFlags(Class<?> type)
static jint getClassAccessFlags(JNIEnv *env, jclass cls, jclass type)
{
    // return type->clazz->access_flags; // todo
    return JVM_GetClassAccessFlags(env, type);
}

static JNINativeMethod jdk_internal_reflect_Reflection_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "getCallerClass", "()Ljava/lang/Class;", getCallerClass },
        // { "getCallerClass", "(I)Ljava/lang/Class;", getCallerClass0 },
        { "getClassAccessFlags", "(Ljava/lang/Class;)I", getClassAccessFlags },
        { NULL }
};

// public static native Object get(Object array, int index)
//              throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static jobject getArrayElement(JNIEnv *env, jclass cls, jobject array, jint index)
{
    return JVM_GetArrayElement(env, array, index);
}

// public static native void set(Object array, int index, Object value)
//               throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void setArrayElement(JNIEnv *env, jclass cls, jobject array, jint index, jobject value)
{
    JVM_SetArrayElement(env, array, index, value);
}

// public static native int getLength(Object array) throws IllegalArgumentException;
static jint getArrayLength(JNIEnv *env, jclass cls, jobject array)
{
    return JVM_GetArrayLength(env, array);
}

// private static native Object newArray(Class<?> componentType, int length)
//                  throws NegativeArraySizeException;
static jobject newArray(JNIEnv *env, jclass cls, jclass componentType, jint length)
{
    return JVM_NewArray(env, componentType, length);
}

static JNINativeMethod java_lang_reflect_Array_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "get", _OBJ "I)" OBJ, getArrayElement },
        { "set", _OBJ "ILjava/lang/Object;)V", setArrayElement },
        { "getLength", _OBJ ")I", getArrayLength },
        { "newArray", _CLS "I)" OBJ, newArray },
        { NULL }
};

/*
 * Returns whether underlying JVM supports lockless CompareAndSet
 * for longs. Called only once and cached in VM_SUPPORTS_LONG_CAS.
 *
 * private static native boolean VMSupportsCS8();
 */
static jboolean VMSupportsCS8(JNIEnv *env, jclass cls)
{
/*
 * 	stack := frame.OperandStack()
	stack.PushBoolean(false) // todo sync/atomic
 */
    return false; // todo  sync/atomic
}

static JNINativeMethod java_util_concurrent_atomic_AtomicLong_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "VMSupportsCS8", "()Z", VMSupportsCS8 },
};

static JNINativeMethod java_security_AccessController_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "getStackAccessControlContext", "()Ljava/security/AccessControlContext;", JVM_GetStackAccessControlContext },
        { "getInheritedAccessControlContext", "()Ljava/security/AccessControlContext;", JVM_GetInheritedAccessControlContext },
        { NULL }
};

// private static native Object invoke0(Method method, Object o, Object[] os);
static jobject invokeMethod(JNIEnv *env, jclass cls, jobject method, jobject o, jobjectArray os)
{
    assert(method != NULL);
    assert(is_array_object((jref) os));
    return JVM_InvokeMethod(env, method, o, os);
}

static JNINativeMethod jdk_internal_reflect_NativeMethodAccessorImpl_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "invoke0", "(Ljava/lang/reflect/Method;" OBJ "[" OBJ_ OBJ, invokeMethod },
        { NULL }
};

// private static native int findSignal(String string);
static jint findSignal(JNIEnv *env, jclass cls, jstrRef string)
{
    const char *name = string_to_utf8(string);
    return JVM_FindSignal(name);
    // return 0; // todo
}

// private static native long handle0(int i, long l);
static jlong handleSignal(JNIEnv *env, jclass cls, jint i, jlong l)
{
    return 0; // todo
}

// private static native void raise0(int i);
static void raiseSignal(JNIEnv *env, jclass cls, jint i)
{
    JVM_RaiseSignal(i);
}

static JNINativeMethod jdk_internal_misc_Signal_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "findSignal0", _STR_ "I", findSignal },
        { "handle0", "(IJ)J", handleSignal },
        { "raise0", "(I)V", raiseSignal },
        { NULL }
};

// private static native void initialize();
static void initialize(JNIEnv *env, jclass cls)
{
    // todo
    Class *sys = load_boot_class(S(java_lang_System));
    init_class(sys);

    if (IS_JDK9_PLUS) {
        Method *m = lookup_static_method(sys, "initPhase1", S(___V));
        assert(m != NULL);
        exec_java_func(m, NULL);

        //   todo
        printvm("initPhase2 is not implement\n");
//    m = sys->lookupStaticMethod("initPhase2", "(ZZ)I");
//    assert(m != NULL);
//    jint ret = slot::getInt(execJavaFunc(m, {slot::islot(1), slot::islot(1)}));
//    assert(ret == 0); // 等于0表示成功

        m = lookup_static_method(sys, "initPhase3", S(___V));
        assert(m != NULL);
        exec_java_func(m, NULL);
    } else {
        Method *m = lookup_static_method(sys, "initializeSystemClass", S(___V));
        assert(m != NULL);
        exec_java_func(m, NULL);
    }
}

// public static void initializeFromArchive(Class c);
static void initializeFromArchive(JNIEnv *env, jclass cls, jref c)
{
    return; // todo
    JVM_PANIC("initializeFromArchive"); // todo
}

static jlong getRandomSeedForCDSDump(JNIEnv *env, jclass cls)
{
//    printvm("getRandomSeedForCDSDump\n");
    return 100; // todo
    JVM_PANIC("getRandomSeedForCDSDump"); // todo
}

static JNINativeMethod jdk_internal_misc_VM_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "initialize", "()V", initialize },
        { "initializeFromArchive", "(Ljava/lang/Class;)V", initializeFromArchive },
        { "getRandomSeedForCDSDump", "()J", getRandomSeedForCDSDump },
        { NULL }
};


// private native int getSize0(Object constantPoolOop);
static jint constantPoolGetSize(JNIEnv *env, jref this, jref constantPoolOop)
{
    // todo  this 和 constantPoolOop 是不是同一对象????????????????????
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp->size; // JVM_ConstantPoolGetSize
}

// private native Class getClassAt0(Object constantPoolOop, int i);
static jclsref constantPoolGetClassAt(JNIEnv *env, jref this, jref constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return resolve_class(cp, (u2)i)->java_mirror;
}

// private native long getLongAt0(Object constantPoolOop, int i);
static jlong constantPoolGetLongAt(JNIEnv *env, jref this, jref constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return cp_get_long(cp, (u2) i);
}

// private native String getUTF8At0(Object constantPoolOop, int i);
static jstrref constantPoolGetUTF8At(JNIEnv *env, jref this, jref constantPoolOop, jint i)
{
    ConstantPool *cp = (ConstantPool *) constantPoolOop;
    return resolve_string(cp, i);
}

static JNINativeMethod jdk_internal_reflect_ConstantPool_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "getSize0", "(Ljava/lang/Object;)I", constantPoolGetSize },
        { "getClassAt0", "(Ljava/lang/Object;I)" CLS, constantPoolGetClassAt },
        { "getLongAt0", "(Ljava/lang/Object;I)J", constantPoolGetLongAt },
        { "getUTF8At0", "(Ljava/lang/Object;I)" STR, constantPoolGetUTF8At },
        { NULL }
};

/*
 * Gather the VM and command line properties and return as a String[].
 * The array indices are alternating key/value pairs
 * supplied by the VM including those defined on the command line
 * using -Dkey=value that may override the platform defined value.
 *
 * Note: The platform encoding must have been set.
 *
 * Class:     jdk_internal_util_SystemProps_Raw
 * Method:    vmProperties
 * Signature: ()[Ljava/lang/String;
 */
// private static native String[] vmProperties();
static jref vmProperties(JNIEnv *env, jclass cls)
{
    jarrref prop_array = alloc_string_array(g_properties_count*2);
    int j = 0;
    for (int i = 0; i < g_properties_count; i++) {
        array_set_ref(prop_array, j++, alloc_string(g_properties[i].name));
        array_set_ref(prop_array, j++, alloc_string(g_properties[i].value));
    }
    return prop_array;
}

/*
 * Gather the system properties and return as a String[].
 * The first FIXED_LENGTH entries are the platform defined property values, no names.
 * The remaining array indices are alternating key/value pairs
 * supplied by the VM including those defined on the command line
 * using -Dkey=value that may override the platform defined value.
 * The caller is responsible for replacing platform provided values as needed.
 *
 * Class:     jdk_internal_util_SystemProps_Raw
 * Method:    platformProperties
 * Signature: ()[Ljava/lang/String;
 */
// private static native String[] platformProperties();
static jref platformProperties(JNIEnv *env, jclass cls)
{
    // from class jdk/internal/util/SystemProps$Raw
    static const int _display_country_NDX = 0;
    static const int _display_language_NDX = 1 + _display_country_NDX;
    static const int _display_script_NDX = 1 + _display_language_NDX;
    static const int _display_variant_NDX = 1 + _display_script_NDX;
    static const int _file_encoding_NDX = 1 + _display_variant_NDX;
    static const int _file_separator_NDX = 1 + _file_encoding_NDX;
    static const int _format_country_NDX = 1 + _file_separator_NDX;
    static const int _format_language_NDX = 1 + _format_country_NDX;
    static const int _format_script_NDX = 1 + _format_language_NDX;
    static const int _format_variant_NDX = 1 + _format_script_NDX;
    static const int _ftp_nonProxyHosts_NDX = 1 + _format_variant_NDX;
    static const int _ftp_proxyHost_NDX = 1 + _ftp_nonProxyHosts_NDX;
    static const int _ftp_proxyPort_NDX = 1 + _ftp_proxyHost_NDX;
    static const int _http_nonProxyHosts_NDX = 1 + _ftp_proxyPort_NDX;
    static const int _http_proxyHost_NDX = 1 + _http_nonProxyHosts_NDX;
    static const int _http_proxyPort_NDX = 1 + _http_proxyHost_NDX;
    static const int _https_proxyHost_NDX = 1 + _http_proxyPort_NDX;
    static const int _https_proxyPort_NDX = 1 + _https_proxyHost_NDX;
    static const int _java_io_tmpdir_NDX = 1 + _https_proxyPort_NDX;
    static const int _line_separator_NDX = 1 + _java_io_tmpdir_NDX;
    static const int _os_arch_NDX = 1 + _line_separator_NDX;
    static const int _os_name_NDX = 1 + _os_arch_NDX;
    static const int _os_version_NDX = 1 + _os_name_NDX;
    static const int _path_separator_NDX = 1 + _os_version_NDX;
    static const int _socksNonProxyHosts_NDX = 1 + _path_separator_NDX;
    static const int _socksProxyHost_NDX = 1 + _socksNonProxyHosts_NDX;
    static const int _socksProxyPort_NDX = 1 + _socksProxyHost_NDX;
    static const int _sun_arch_abi_NDX = 1 + _socksProxyPort_NDX;
    static const int _sun_arch_data_model_NDX = 1 + _sun_arch_abi_NDX;
    static const int _sun_cpu_endian_NDX = 1 + _sun_arch_data_model_NDX;
    static const int _sun_cpu_isalist_NDX = 1 + _sun_cpu_endian_NDX;
    static const int _sun_io_unicode_encoding_NDX = 1 + _sun_cpu_isalist_NDX;
    static const int _sun_jnu_encoding_NDX = 1 + _sun_io_unicode_encoding_NDX;
    static const int _sun_os_patch_level_NDX = 1 + _sun_jnu_encoding_NDX;
    static const int _sun_stderr_encoding_NDX = 1 + _sun_os_patch_level_NDX;
    static const int _sun_stdout_encoding_NDX = 1 + _sun_stderr_encoding_NDX;
    static const int _user_dir_NDX = 1 + _sun_stdout_encoding_NDX;
    static const int _user_home_NDX = 1 + _user_dir_NDX;
    static const int _user_name_NDX = 1 + _user_home_NDX;
    static const int FIXED_LENGTH = 1 + _user_name_NDX;

    jarrref prop_array = alloc_string_array(FIXED_LENGTH);
    // todo 具体赋值

    for (int i = 0; i < FIXED_LENGTH; i++) {
        array_set_ref(prop_array, i, NULL);
    }
    return prop_array;
}

static JNINativeMethod jdk_internal_util_SystemProps$Raw_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "vmProperties", "()[Ljava/lang/String;", vmProperties },
        { "platformProperties", "()[Ljava/lang/String;", platformProperties },
        { NULL }
};

// static
static jlong setErrorMode(jlong l)
{
    return 0; // todo
}

static JNINativeMethod sun_io_Win32ErrorMode_NativeMethods[] = {
        JNINativeMethod_registerNatives,
        { "setErrorMode", "(J)J", setErrorMode },
        { NULL }
};

/*
 * VM support where maxDepth == -1 to request entire stack dump
 *
 * private static native ThreadInfo[] dumpThreads0(
 *                      long[] ids, boolean lockedMonitors, boolean lockedSynchronizers, int maxDepth);
 *
 */
static jref dumpThreads0(JNIEnv *env, jclass cls, jref _ids, jboolean lockedMonitors, jboolean lockedSynchronizers, jint maxDepth)
{
    Object *thread_infos;

    Class *ac = loadArrayClass0("[Ljava/lang/management/ThreadInfo;");
    if (_ids == NULL) { // dump all threads
        thread_infos = alloc_array(ac, g_all_threads_count);

        for (int i = 0; i < g_all_threads_count; i++) {
            Thread *t = g_all_threads[i];
            Object *thread_info = to_java_lang_management_ThreadInfo(t, lockedMonitors, lockedSynchronizers, maxDepth);
            array_set_ref(thread_infos, i, thread_info);
        }
    } else {
        jarrRef ids = (jarrRef)(_ids);
        thread_infos = alloc_array(ac, ids->arr_len);

        for (int i = 0; i < ids->arr_len; i++) {
            jlong id = array_get(jlong, ids, i);
            Thread *t = thread_from_id(id);
            assert(t != NULL);
            Object *thread_info = to_java_lang_management_ThreadInfo(t, lockedMonitors, lockedSynchronizers, maxDepth);
            array_set_ref(thread_infos, i, thread_info);
        }
    }

    return thread_infos;
}

// static JNINativeMethod sun_management_ThreadImplNativeMethods[] = {
//         JNINativeMethod_registerNatives,
//         { "dumpThreads0", "([JZZ)[Ljava/lang/management/ThreadInfo;", dumpThreads0 },
//         { NULL }
// };

void register_natives(const char *class_name, JNINativeMethod *methods);

void init_native()
{
    register_natives("java/lang/Object", java_lang_Object_NativeMethods);
    register_natives("java/lang/ClassLoader", java_lang_ClassLoader_NativeMethods);
    register_natives("java/lang/System", java_lang_System_NativeMethods);
    register_natives("java/lang/Float", java_lang_Float_NativeMethods);
    register_natives("java/lang/Double", java_lang_Double_NativeMethods);
    register_natives("java/lang/String", java_lang_String_NativeMethods);
    register_natives("java/lang/Package", java_lang_Package_NativeMethods);
    register_natives("java/lang/Thread", java_lang_Thread_NativeMethods);
    register_natives("java/lang/Throwable", java_lang_Throwable_NativeMethods);
    register_natives("java/lang/StringUTF16", java_lang_StringUTF16_NativeMethods);
    register_natives("java/lang/NullPointerException", java_lang_NullPointerException_NativeMethods);
    register_natives("java/lang/StackTraceElement", java_lang_StackTraceElement_NativeMethods);
    register_natives("java/lang/Runtime", java_lang_Runtime_NativeMethods);
    register_natives("java/lang/Module", java_lang_Module_NativeMethods);
    register_natives("java/lang/Shutdown", java_lang_Shutdown_NativeMethods);
    register_natives("java/lang/reflect/Array", java_lang_reflect_Array_NativeMethods);
    register_natives("java/util/concurrent/atomic/AtomicLong", java_util_concurrent_atomic_AtomicLong_NativeMethods);
    register_natives("java/security/AccessController", java_security_AccessController_NativeMethods);
    register_natives("jdk/internal/reflect/NativeConstructorAccessorImpl", sun_reflect_NativeConstructorAccessorImpl_NativeMethods);
    register_natives("jdk/internal/reflect/Reflection", jdk_internal_reflect_Reflection_NativeMethods);
    register_natives("jdk/internal/reflect/NativeMethodAccessorImpl", jdk_internal_reflect_NativeMethodAccessorImpl_NativeMethods);
    register_natives("jdk/internal/misc/Signal", jdk_internal_misc_Signal_NativeMethods);
    register_natives("jdk/internal/misc/VM", jdk_internal_misc_VM_NativeMethods);
    register_natives("jdk/internal/reflect/ConstantPool", jdk_internal_reflect_ConstantPool_NativeMethods);
    register_natives("jdk/internal/util/SystemProps$Raw", jdk_internal_util_SystemProps$Raw_NativeMethods);

    register_natives("sun/io/Win32ErrorMode", sun_io_Win32ErrorMode_NativeMethods);
    register_natives("sun/management/ThreadImpl", (JNINativeMethod[]) {
        JNINativeMethod_registerNatives,
        { "dumpThreads0", "([JZZ)[Ljava/lang/management/ThreadInfo;", dumpThreads0 },
        { NULL }
    });

#undef R
#define R(method_name) void method_name(); /* declare */ method_name() /* invoke */

    // register all native methods
    R(java_lang_Class_registerNatives);

    R(java_lang_invoke_MethodHandle_registerNatives);
    R(java_lang_invoke_MethodHandleNatives_registerNatives);

    R(java_io_FileDescriptor_registerNatives);
    R(java_io_FileInputStream_registerNatives);
    R(java_io_FileOutputStream_registerNatives);
    R(java_io_WinNTFileSystem_registerNatives);

    R(sun_misc_Unsafe_registerNatives);
#undef R
}

static struct native_owned_class {
    const utf8_t *class_name;
    JNINativeMethod *methods;

    struct native_owned_class *next;
} *head = NULL;

void register_natives(const char *class_name, JNINativeMethod *methods)
{
    assert(class_name != NULL && methods != NULL);

    struct native_owned_class *c = vm_malloc(sizeof(*c));
    c->class_name = class_name;
    c->methods = methods;
    c->next = head;
    head = c;
}

JNINativeMethod *find_native_method(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != NULL && method_name != NULL);

    for (struct native_owned_class *c = head; c != NULL; c = c->next) {
        if (!utf8_equals(c->class_name, class_name)) 
            continue;
            
        for (JNINativeMethod *m = c->methods; m->name != NULL; m++) {
            if (utf8_equals(m->name, method_name) && utf8_equals(m->signature, method_descriptor)) {
                return m;
            }
        }
        return NULL; // not find
    }

    return NULL; // not find
}
