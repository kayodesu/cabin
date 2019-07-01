/*
 * Author: Jia Yang
 */

#include <ctime>
#include "../../registry.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/heap/ArrayObject.h"
#include "../../../interpreter/interpreter.h"
#include "../../../rtda/thread/Thread.h"
#include "../../../rtda/heap/StringObject.h"

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
    auto libname = frame->getLocalAsRef<StringObject>(0);
    if (libname == nullptr) {
        thread_throw_null_pointer_exception(); // todo
    }

    const char *name = libname->getUtf8Value();
    char mapping_name[strlen(name) + 5];;
    strcpy(mapping_name, name);
    strcat(mapping_name, ".dll"); // todo ...........................
    frame->pushr(StringObject::newInst(mapping_name));  // todo
}

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(Frame *frame)
{
    jref src = frame->getLocalAsRef(0);
    jint src_pos = frame->getLocalAsInt(1);
    jref dest = frame->getLocalAsRef(2);
    jint dest_pos = frame->getLocalAsInt(3);
    jint length = frame->getLocalAsInt(4);

    assert(dest->isArray());
    assert(src->isArray());
    ArrayObject::copy((ArrayObject *) dest, dest_pos, (ArrayObject *) src, src_pos, length);
}

// public static native int identityHashCode(Object x);
static void identityHashCode(Frame *frame)
{
    jref x = frame->getLocalAsRef(0);
    frame->pushi((jint) (intptr_t) x);
}

/*
 * System properties. The following properties are guaranteed to be defined:
 * java.version         Java version number
 * java.vendor          Java vendor specific string
 * java.vendor.url      Java vendor URL
 * java.home            Java installation directory
 * java.class.version   Java class version number
 * java.class.path      Java classpath
 * os.name              Operating System Name
 * os.arch              Operating System Architecture
 * os.version           Operating System Version
 * file.separator       File separator ("/" on Unix)
 * path.separator       Path separator (":" on Unix)
 * line.separator       Line separator ("\n" on Unix)
 * user.name            User account name
 * user.home            User home directory
 * user.dir             User's current working directory
 */
// private static native Properties initProperties(Properties props);
static void initProperties(Frame *frame)
{
    const char *sys_props[][2] = {  // todo
            { "java.version",         "1.8.0" },  // todo
            { "java.vendor",          "Jia Yang" }, // todo "jvm.go"
            { "java.vendor.url",      "doesn't have"}, // todo "https://github.com/zxh0/jvm.go"
            { "java.home",            R"(C:\Program Files\Java\jdk1.8.0_162)"}, // options.AbsJavaHome // todo
            { "java.class.version",   "52.0"}, // todo
            { "java.class.path",      R"(C:\Program Files\Java\jdk1.8.0_162\lib)"}, // heap.BootLoader().ClassPath().String() // todo
//            { "java.awt.graphicsenv", "sun.awt.CGraphicsEnvironment"}, // todo
            { "os.name",              "" },   // todo runtime.GOOS
            { "os.arch",              "" }, // todo runtime.GOARCH
            { "os.version",           "" },             // todo
            { "file.separator",       "\\" },            // todo os.PathSeparator
            { "path.separator",       ";" },            // todo os.PathListSeparator
            { "line.separator",       "\n" }, // System.out.println最后输出换行符就会用到这个  // todo
            { "user.name",            "" },             // todo
            { "user.home",            "" },             // todo
            { "user.dir",             "." },            // todo
            { "user.country",         "CN" },           // todo
            { "file.encoding",        "UTF-8" },
            { "sun.stdout.encoding",  "UTF-8" },
            { "sun.stderr.encoding",  "UTF-8" },
    };

    Object *props = frame->getLocalAsRef(0);

    // todo init
    Method *set_property = props->clazz->lookupInstMethod(
            "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    for (auto &sys_prop : sys_props) {
        slot_t args[] = {
                (slot_t) props,
                (slot_t) StringObject::newInst(sys_prop[0]),
                (slot_t) StringObject::newInst(sys_prop[1])
        };
        exec_java_func(set_property, args);
    }

    // 返回参数
    frame->pushr(props);
}

// private static native void setIn0(InputStream in);
static void setIn0(Frame *frame)
{
    jref in = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->setStaticFieldValue(c->lookupField("in", "Ljava/io/InputStream;"), (slot_t *) &in);
}

// private static native void setOut0(PrintStream out);
static void setOut0(Frame *frame)
{
    jref out = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->setStaticFieldValue(c->lookupField("out", "Ljava/io/PrintStream;"), (slot_t *) &out);
}

// private static native void setErr0(PrintStream err);
static void setErr0(Frame *frame)
{
    jref err = frame->getLocalAsRef(0);
    Class *c = frame->method->clazz;
    c->setStaticFieldValue(c->lookupField("err", "Ljava/io/PrintStream;"), (slot_t *) &err);
}

/*
 * 返回最准确的可用系统计时器的当前值，以毫微秒为单位。
此方法只能用于测量已过的时间，与系统或钟表时间的其他任何时间概念无关。
 返回值表示从某一固定但任意的时间算起的毫微秒数（或许从以后算起，所以该值可能为负）。
 此方法提供毫微秒的精度，但不是必要的毫微秒的准确度。它对于值的更改频率没有作出保证。
 在取值范围大于约 292 年（263 毫微秒）的连续调用的不同点在于：由于数字溢出，将无法准确计算已过的时间。
todo
 *
 * public static native long nanoTime();
 */
static void nanoTime(Frame *frame)
{
    jvm_abort("error\n");
    // todo
    /*
     * 	nanoTime := time.Now().UnixNano()
	stack := frame.OperandStack()
	stack.PushLong(nanoTime)
     */
    frame->pushl((jlong)1);
}

// public static native long currentTimeMillis();
static void currentTimeMillis(Frame *frame)
{
    jvm_abort("error\n"); // todo
}

void java_lang_System_registerNatives()
{
#undef C
#define C "java/lang/System",
    register_native_method(C"mapLibraryName", "(Ljava/lang/String;)" LSTR, mapLibraryName);
    register_native_method(C"arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    register_native_method(C"identityHashCode", "(Ljava/lang/Object;)I", identityHashCode);
    register_native_method(C"initProperties", "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    register_native_method(C"setIn0", "(Ljava/io/InputStream;)V", setIn0);
    register_native_method(C"setOut0", "(Ljava/io/PrintStream;)V", setOut0);
    register_native_method(C"setErr0", "(Ljava/io/PrintStream;)V", setErr0);

    register_native_method(C"nanoTime", "()J", nanoTime);
    register_native_method(C"currentTimeMillis", "()J", currentTimeMillis);
}
