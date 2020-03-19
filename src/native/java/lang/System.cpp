/*
 * Author: kayo
 */

#include <chrono>
#include "../../registry.h"
#include "../../../objects/object.h"
#include "../../../interpreter/interpreter.h"
#include "../../../runtime/Frame.h"
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
static void mapLibraryName(Frame *frame)
{
    auto libname = frame->getLocalAsRef(0);
    if (libname == nullptr) {
        throw NullPointerException(); // todo
    }

    const char *name = libname->toUtf8();
    char mapping_name[strlen(name) + 5];;
    strcpy(mapping_name, name);
    strcat(mapping_name, ".dll"); // todo ...........................
    frame->pushr(newString(mapping_name));  // todo
}

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(Frame *frame)
{
    jref src = frame->getLocalAsRef(0);
    jint src_pos = frame->getLocalAsInt(1);
    jref dest = frame->getLocalAsRef(2);
    jint dest_pos = frame->getLocalAsInt(3);
    jint length = frame->getLocalAsInt(4);

    assert(dest->isArrayObject());
    assert(src->isArrayObject());
    Array::copy((Array *) dest, dest_pos, (Array *) src, src_pos, length);
}

// public static native int identityHashCode(Object x);
static void identityHashCode(Frame *frame)
{
    jref x = frame->getLocalAsRef(0);
    frame->pushi((jint) (intptr_t) x); // todo 实现错误。改成当前的时间如何。
}

// private static native Properties initProperties(Properties props);
static void initProperties(Frame *frame)
{
    Object *props = frame->getLocalAsRef(0);

    // todo init
    Method *setProperty = props->clazz->lookupInstMethod(
            "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    for (auto &prop : g_properties) {
        assert(prop.first != nullptr && prop.second != nullptr);
        execJavaFunc(setProperty, props, newString(prop.first), newString(prop.second));
    }

    frame->pushr(props);
}

// private static native void setIn0(InputStream in);
static void setIn0(Frame *frame)
{
    jref in = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->lookupStaticField("in", "Ljava/io/InputStream;")->staticValue.r = in;
}

// private static native void setOut0(PrintStream out);
static void setOut0(Frame *frame)
{
    jref out = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->lookupStaticField("out", "Ljava/io/PrintStream;")->staticValue.r = out;
}

// private static native void setErr0(PrintStream err);
static void setErr0(Frame *frame)
{
    jref err = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->lookupStaticField("err", "Ljava/io/PrintStream;")->staticValue.r = err;
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
static void nanoTime(Frame *frame)
{
    // todo
    frame->pushl(duration_cast<nanoseconds>(high_resolution_clock ::now().time_since_epoch()).count());
}

// 毫秒
// public static native long currentTimeMillis();
static void currentTimeMillis(Frame *frame)
{
    // todo
    frame->pushl(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
}

void java_lang_System_registerNatives()
{
#undef C
#define C "java/lang/System",
    registerNative(C "mapLibraryName", "(Ljava/lang/String;)" LSTR, mapLibraryName);
    registerNative(C "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    registerNative(C "identityHashCode", "(Ljava/lang/Object;)I", identityHashCode);
    registerNative(C "initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    registerNative(C "setIn0", "(Ljava/io/InputStream;)V", setIn0);
    registerNative(C "setOut0", "(Ljava/io/PrintStream;)V", setOut0);
    registerNative(C "setErr0", "(Ljava/io/PrintStream;)V", setErr0);

    registerNative(C "nanoTime", "()J", nanoTime);
    registerNative(C "currentTimeMillis", "()J", currentTimeMillis);
}
