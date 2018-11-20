/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../rtda/heap/jobject.h"

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(struct stack_frame *frame)
{
    struct jobject *src = slot_getr(frame->local_vars);
    ARROBJ_CHECK(src);
    jint src_pos = slot_geti(frame->local_vars + 1);
    struct jobject *dest = slot_getr(frame->local_vars + 2);
    ARROBJ_CHECK(dest);
    jint dest_pos = slot_geti(frame->local_vars + 3);
    jint length = slot_geti(frame->local_vars + 4);

    jarrobj_copy(dest, dest_pos, src, src_pos, length);
}

// private static native Properties initProperties(Properties props);
static void initProperties(struct stack_frame *frame)
{
    char *sys_props[][2] = {  // todo
            { "java.version",          "1.8.0" },  // todo
            {  "java.vendor",          "" },
            { "java.vendor.url",      ""},
            { "java.home",            "" }, // options.AbsJavaHome
            { "java.class.version",   "52.0"},
            { "java.class.path",      "" }, // heap.BootLoader().ClassPath().String()
            { "java.awt.graphicsenv", "sun.awt.CGraphicsEnvironment"},
            { "os.name",              "" },   // todo runtime.GOOS
            { "os.arch",              "" }, // todo runtime.GOARCH
            { "os.version",           "" },             // todo
            { "file.separator",       "/" },            // todo os.PathSeparator
            { "path.separator",       ":" },            // todo os.PathListSeparator
            { "line.separator",       "\n" },           // todo
            { "user.name",            "" },             // todo
            { "user.home",            "" },             // todo
            { "user.dir",             "." },            // todo
            { "user.country",         "CN" },           // todo
            { "file.encoding",        "UTF-8" },
            { "sun.stdout.encoding",  "UTF-8" },
            { "sun.stderr.encoding",  "UTF-8" },
    };

    struct jobject *props = slot_getr(frame->local_vars);//frame->getLocalVar(0).getRef();

    // todo init
    struct jmethod *set_property = jclass_lookup_instance_method(
            props->jclass, "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
    if (set_property == NULL) {
        jvm_abort("error\n"); // todo
    }

//    int props_count = sizeof(sys_props) / sizeof(*sys_props);

    for (int i = 0; i < sizeof(sys_props) / sizeof(*sys_props); i++) {
        struct slot args[] = {
                rslot(props),
                rslot(jstrobj_create(props->jclass->loader, sys_props[i][0])),
                rslot(jstrobj_create(props->jclass->loader, sys_props[i][1]))
        };
        // todo 不能再循环中调用jthread_invoke_method，参见jthread_invoke_method的注释.
//        jthread_invoke_method(frame->thread, set_property, args);
    }

    // 返回参数
    os_pushr(frame->operand_stack, props);
}

// private static native void setIn0(InputStream in);
static void setIn0(struct stack_frame *frame) {
    jvm_abort("error\n");
//    jref in = frame->getLocalVar(0).getRef();
//    JClass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("in", "Ljava/io/InputStream;", Slot(in));
}

// private static native void setOut0(PrintStream out);
static void setOut0(struct stack_frame *frame) {
    jvm_abort("error\n");
//    jref out = frame->getLocalVar(0).getRef();
//    JClass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("out", "Ljava/io/PrintStream;", Slot(out));
}

// private static native void setErr0(PrintStream err);
static void setErr0(struct stack_frame *frame) {
    jvm_abort("error\n");
//    jref err = frame->getLocalVar(0).getRef();
//    struct jclass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("err", "Ljava/io/PrintStream;", Slot(err));
}

// public static native long nanoTime();
static void nanoTime(struct stack_frame *frame) {
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
    register_native_method("java/lang/System", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/System", "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    register_native_method("java/lang/System", "initProperties",
                         "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    register_native_method("java/lang/System", "setIn0", "(Ljava/io/InputStream;)V", setIn0);
    register_native_method("java/lang/System", "setOut0", "(Ljava/io/PrintStream;)V", setOut0);
    register_native_method("java/lang/System", "setErr0", "(Ljava/io/PrintStream;)V", setErr0);

    register_native_method("java/lang/System", "nanoTime", "()J", nanoTime);
}
