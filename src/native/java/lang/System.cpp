/*
 * Author: Yo Ka
 */

#include <chrono>
#include "../../jni_inner.h"
#include "../../../objects/object.h"
#include "../../../objects/field.h"
#include "../../../objects/array_object.h"
#include "../../../interpreter/interpreter.h"
#include "../../../runtime/frame.h"
#include "../../../runtime/thread_info.h"
#include "../../../objects/class.h"

using namespace std;
using namespace chrono;

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
static jstrref mapLibraryName(JNIEnv *env, jclass clazz, jstrref libname)
{
    if (libname == nullptr) {
        throw NullPointerException(); // todo
    }

    const char *name = libname->toUtf8();
    char mapping_name[strlen(name) + 5];;
    strcpy(mapping_name, name);
    strcat(mapping_name, ".dll"); // todo ...........................
    return newString(mapping_name);  // todo
}

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(JNIEnv *env, jclass clazz, jref src, jint src_pos, jref dest, jint dest_pos, jint length)
{
    assert(dest->isArrayObject());
    assert(src->isArrayObject());
    Array::copy((Array *) dest, dest_pos, (Array *) src, src_pos, length);
}

// public static native int identityHashCode(Object x);
static jint identityHashCode(JNIEnv *env, jclass clazz, jobject x)
{
    return (jint) (intptr_t) x; // todo 实现错误。改成当前的时间如何。
}

// private static native Properties initProperties(Properties props);
static jref initProperties(JNIEnv *env, jclass clazz, jref props)
{
    // todo init
    Method *setProperty = props->clazz->lookupInstMethod(
            "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    for (auto &prop : g_properties) {
        assert(prop.first != nullptr && prop.second != nullptr);
        execJavaFunc(setProperty, props, newString(prop.first), newString(prop.second));
    }

    return props;
}

// private static native void setIn0(InputStream in);
static void setIn0(JNIEnv *env, jclass clazz, jobject in)
{
    jfieldID fid = env->GetFieldID(clazz, "in", "Ljava/io/InputStream;");
    env->SetStaticObjectField(clazz, fid, in);
    
    // clazz->lookupStaticField("in", "Ljava/io/InputStream;")->staticValue.r = in;
}

// private static native void setOut0(PrintStream out);
static void setOut0(JNIEnv *env, jclass clazz, jobject out)
{
    jfieldID fid = env->GetFieldID(clazz, "out", "Ljava/io/PrintStream;");
    env->SetStaticObjectField(clazz, fid, out);

    // clazz->lookupStaticField("out", "Ljava/io/PrintStream;")->staticValue.r = out;
}

// private static native void setErr0(PrintStream err);
static void setErr0(JNIEnv *env, jclass clazz, jobject err)
{
    jfieldID fid = env->GetFieldID(clazz, "err", "Ljava/io/PrintStream;");
    env->SetStaticObjectField(clazz, fid, err);

    // clazz->lookupStaticField("err", "Ljava/io/PrintStream;")->staticValue.r = err;
}

/*
 * 返回最准确的可用系统计时器的当前值，以毫微秒为单位。
 * 此方法只能用于测量已过的时间，与系统或钟表时间的其他任何时间概念无关。
 * 返回值表示从某一固定但任意的时间算起的毫微秒数（或许从以后算起，所以该值可能为负）。
 * 此方法提供毫微秒的精度，但不是必要的毫微秒的准确度。它对于值的更改频率没有作出保证。
 * 在取值范围大于约 292 年（263 毫微秒）的连续调用的不同点在于：由于数字溢出，将无法准确计算已过的时间。
 *
 * public static native long nanoTime();
 */
static jlong nanoTime(JNIEnv *env, jclass clazz)
{
    // todo
    return duration_cast<nanoseconds>(high_resolution_clock ::now().time_since_epoch()).count();
}

// 毫秒
// public static native long currentTimeMillis();
static jlong currentTimeMillis(JNIEnv *env, jclass clazz)
{
    // todo
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "mapLibraryName", _STR_ STR, (void *) mapLibraryName },
        { "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", (void *) arraycopy },
        { "identityHashCode", "(Ljava/lang/Object;)I", (void *) identityHashCode },
        { "initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", (void *) initProperties },

        { "setIn0", "(Ljava/io/InputStream;)V", (void *) setIn0 },
        { "setOut0", "(Ljava/io/PrintStream;)V", (void *) setOut0 },
        { "setErr0", "(Ljava/io/PrintStream;)V", (void *) setErr0 },

        { "nanoTime", "()J", (void *) nanoTime },
        { "currentTimeMillis", "()J", (void *) currentTimeMillis },
};


void java_lang_System_registerNatives()
{
    registerNatives("java/lang/System", methods, ARRAY_LENGTH(methods));
}
