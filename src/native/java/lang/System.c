/*
 * Author: Jia Yang
 */

#include <time.h>
#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../rtda/heap/jobject.h"

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
static void mapLibraryName(struct stack_frame *frame)
{
    jref libname = slot_getr(frame->local_vars);
    if (libname == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }
#ifdef JVM_DEBUG
    JOBJECT_CHECK_STROBJ(libname);
#endif
//    jvm_abort("%s\n", jstrobj_value(libname));
    const char *name = jstrobj_value(libname);
    char mapping_name[strlen(name) + 5];;
    strcpy(mapping_name, name);
    strcat(mapping_name, ".dll"); // todo ...........................
//    printvm("mapLibraryName, %s\n", mapping_name);
    os_pushr(frame->operand_stack, jstrobj_create(mapping_name));  // todo
}

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(struct stack_frame *frame)
{
    struct jobject *src = slot_getr(frame->local_vars);
    jint src_pos = slot_geti(frame->local_vars + 1);
    struct jobject *dest = slot_getr(frame->local_vars + 2);
#ifdef JVM_DEBUG
    JOBJECT_CHECK_ARROBJ(src);
    JOBJECT_CHECK_ARROBJ(dest);
#endif
    jint dest_pos = slot_geti(frame->local_vars + 3);
    jint length = slot_geti(frame->local_vars + 4);

    jarrobj_copy(dest, dest_pos, src, src_pos, length);
}

// public static native int identityHashCode(Object x);
static void identityHashCode(struct stack_frame *frame)
{
    jref x = slot_getr(frame->local_vars);
    os_pushi(frame->operand_stack, (jint) (intptr_t) x);
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
static void initProperties(struct stack_frame *frame)
{
    char *sys_props[][2] = {  // todo
            { "java.version",         "1.8.0" },  // todo
            { "java.vendor",          "Jia Yang" }, // todo "jvm.go"
            { "java.vendor.url",      "doesn't have"}, // todo "https://github.com/zxh0/jvm.go"
            { "java.home",            "C:\\Program Files\\Java\\jdk1.8.0_162" }, // options.AbsJavaHome // todo
            { "java.class.version",   "52.0"}, // todo
            { "java.class.path",      "C:\\Program Files\\Java\\jdk1.8.0_162\\lib" }, // heap.BootLoader().ClassPath().String() // todo
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

    struct jobject *props = slot_getr(frame->local_vars);

    // todo init
    struct jmethod *set_property = jclass_lookup_instance_method(
            props->jclass, "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

    for (int i = 0; i < sizeof(sys_props) / sizeof(*sys_props); i++) {
        struct slot args[] = {
                rslot(props),
                rslot((jref) jstrobj_create(sys_props[i][0])),
                rslot((jref) jstrobj_create(sys_props[i][1]))
        };

        jthread_invoke_method_with_shim(frame->thread, set_property, args, NULL);
    }

    // 返回参数
    os_pushr(frame->operand_stack, props);
}

// private static native void setIn0(InputStream in);
static void setIn0(struct stack_frame *frame)
{
    jref in = slot_getr(frame->local_vars);
    struct slot s = rslot(in);
    set_static_field_value_by_nt(frame->method->jclass, "in", "Ljava/io/InputStream;", &s);
}

// private static native void setOut0(PrintStream out);
static void setOut0(struct stack_frame *frame)
{
    jref out = slot_getr(frame->local_vars);
    struct slot s = rslot(out);
    set_static_field_value_by_nt(frame->method->jclass, "out", "Ljava/io/PrintStream;", &s);
}

// private static native void setErr0(PrintStream err);
static void setErr0(struct stack_frame *frame)
{
    jref err = slot_getr(frame->local_vars);
    struct slot s = rslot(err);
    set_static_field_value_by_nt(frame->method->jclass, "err", "Ljava/io/PrintStream;", &s);
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
static void nanoTime(struct stack_frame *frame)
{
    jvm_abort("error\n");
    // todo
    /*
     * 	nanoTime := time.Now().UnixNano()
	stack := frame.OperandStack()
	stack.PushLong(nanoTime)
     */
    os_pushl(frame->operand_stack, (jlong)1);
}

void java_lang_System_registerNatives()
{
    register_native_method("java/lang/System~registerNatives~()V", registerNatives);

    register_native_method("java/lang/System~mapLibraryName~(Ljava/lang/String;)Ljava/lang/String;", mapLibraryName);
    register_native_method("java/lang/System~arraycopy~(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    register_native_method("java/lang/System~identityHashCode~(Ljava/lang/Object;)I", identityHashCode);
    register_native_method("java/lang/System~initProperties~(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    register_native_method("java/lang/System~setIn0~(Ljava/io/InputStream;)V", setIn0);
    register_native_method("java/lang/System~setOut0~(Ljava/io/PrintStream;)V", setOut0);
    register_native_method("java/lang/System~setErr0~(Ljava/io/PrintStream;)V", setErr0);

    register_native_method("java/lang/System~nanoTime~()J", nanoTime);
}
