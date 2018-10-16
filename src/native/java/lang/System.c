/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(struct stack_frame *frame)
{
#if 0
    jref r = frame->getLocalVar(0).getRef();
    if (r == nullptr) {
        // todo "java.lang.NullPointerException"
        jvmAbort("java.lang.NullPointerException\n");
    }
    auto src = static_cast<JArrayObj *>(r);
    if (!src->getClass()->isArray()) {
        jvmAbort("error\n");
    }

    jint srcPos = frame->getLocalVar(1).getInt();

    r = frame->getLocalVar(2).getRef();
    if (r == nullptr) {
        // todo "java.lang.NullPointerException"
        jvmAbort("java.lang.NullPointerException\n");
    }
    auto dest = static_cast<JArrayObj *>(r);
    if (!dest->getClass()->isArray()) {
        jvmAbort("error\n");
    }

    jint dstPos = frame->getLocalVar(3).getInt();
    jint length = frame->getLocalVar(4).getInt();
    JArrayObj::copy(*dest, dstPos, *src, srcPos, length);
#endif
}

// private static native Properties initProperties(Properties props);
static void initProperties(struct stack_frame *frame)
{
#if 0
    std::string sysProps[][2] = {  // todo
            { "java.version",          "1.8.0" },
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

    JObject *props = frame->getLocalVar(0).getRef();
    // todo init

    // public synchronized Object setProperty(String key, String value)
    auto setProperty = props->getClass()->lookupInstanceMethod(
            "setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
    if (setProperty == nullptr) {
        // todo
        jvmAbort("error\n");
    }

    ClassLoader *loader = props->getClass()->loader;
    for (int i = 0; i < sizeof(sysProps) / sizeof(sysProps[0]); i++) {
        Slot args[] = {
                Slot(props),
                Slot(JStringObj::newJStringObj(loader, strToJstr(sysProps[i][0]))),
                Slot(JStringObj::newJStringObj(loader, strToJstr(sysProps[i][1]))),
        };
        frame->invokeMethod(setProperty, args);
    }

    // 返回参数
    frame->operandStack.push(props);
#endif
}

// private static native void setIn0(InputStream in);
static void setIn0(struct stack_frame *frame) {
//    jref in = frame->getLocalVar(0).getRef();
//    JClass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("in", "Ljava/io/InputStream;", Slot(in));
}

// private static native void setOut0(PrintStream out);
static void setOut0(struct stack_frame *frame) {
//    jref out = frame->getLocalVar(0).getRef();
//    JClass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("out", "Ljava/io/PrintStream;", Slot(out));
}

// private static native void setErr0(PrintStream err);
static void setErr0(struct stack_frame *frame) {
//    jref err = frame->getLocalVar(0).getRef();
//    struct jclass *sysClass = frame->method->jclass;
//
//    sysClass->setFieldValue("err", "Ljava/io/PrintStream;", Slot(err));
}

// public static native long nanoTime();
static void nanoTime(struct stack_frame *frame) {
    // todo
    /*
     * 	nanoTime := time.Now().UnixNano()
	stack := frame.OperandStack()
	stack.PushLong(nanoTime)
     */
    os_pushl(frame->operand_stack, (jlong)1);
}

void java_lang_System_registerNatives(struct stack_frame *frame)
{
    register_native_method("java/lang/System", "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    register_native_method("java/lang/System", "initProperties",
                         "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    register_native_method("java/lang/System", "setIn0", "(Ljava/io/InputStream;)V", setIn0);
    register_native_method("java/lang/System", "setOut0", "(Ljava/io/PrintStream;)V", setOut0);
    register_native_method("java/lang/System", "setErr0", "(Ljava/io/PrintStream;)V", setErr0);

    register_native_method("java/lang/System", "nanoTime", "()J", nanoTime);
}
