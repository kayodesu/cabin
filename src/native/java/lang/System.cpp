/*
 * 实现 java/lang/System 类下的 native方法
 *
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/objectarea/JarrayObj.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"

// public static native void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
static void arraycopy(StackFrame *frame) {
    jreference r = frame->getLocalVars(0)->getReference();//slot_get_reference(frame->local_vars + 0);
    if (r == nullptr) {
        // todo "java.lang.NullPointerException"
        jvmAbort("java.lang.NullPointerException\n");
    }
    auto src = static_cast<JarrayObj *>(r);
    if (!src->getClass()->isArray()) {
        jvmAbort("error\n");
    }

    int srcPos = frame->getLocalVars(1)->getInt();

    r = frame->getLocalVars(2)->getReference();
    if (r == nullptr) {
        // todo "java.lang.NullPointerException"
        jvmAbort("java.lang.NullPointerException\n");
    }
    auto dest = static_cast<JarrayObj *>(r);
    if (!dest->getClass()->isArray()) {
        jvmAbort("error\n");
    }

    int destPos = frame->getLocalVars(3)->getInt();
    int length = frame->getLocalVars(4)->getInt();
    JarrayObj::copy(*dest, destPos, *src, srcPos, length);
}

// private static native Properties initProperties(Properties props);
static void initProperties(StackFrame *frame) {
    Jobject *props = static_cast<Jobject *>(frame->getLocalVars(0)->getReference());
    // todo init
    // public synchronized Object setProperty(String key, String value)
    auto m = props->getClass()->lookupInstanceMethod("setProperty", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");
    if (m == nullptr) {
        // todo
        jvmAbort("error\n");
    }
//    Slot args[3] = {
//            Slot::referenceSlot(props),
//    };
//    frame->invokeMethod(m, args);
    frame->operandStack.push(props);
}

// private static native void setIn0(InputStream in);
static void setIn0(StackFrame *frame) {
    jreference in = frame->getLocalVars(0)->getReference();
    Jclass *sysClass = frame->method->jclass;

    Jvalue v;
    v.r = in;
    sysClass->setStaticField("in", "Ljava/io/InputStream;", v);
}

// private static native void setOut0(PrintStream out);
static void setOut0(StackFrame *frame) {
    jreference out = frame->getLocalVars(0)->getReference();
    Jclass *sysClass = frame->method->jclass;

    Jvalue v;
    v.r = out;
    sysClass->setStaticField("out", "Ljava/io/PrintStream;", v);
}

// private static native void setErr0(PrintStream err);
static void setErr0(StackFrame *frame) {
    jreference err = frame->getLocalVars(0)->getReference();
    Jclass *sysClass = frame->method->jclass;

    Jvalue v;
    v.r = err;
    sysClass->setStaticField("err", "Ljava/io/PrintStream;", v);
}

// public static native long nanoTime();
static void nanoTime(StackFrame *frame) {
    // todo
    /*
     * 	nanoTime := time.Now().UnixNano()
	stack := frame.OperandStack()
	stack.PushLong(nanoTime)
     */
    frame->operandStack.push((jlong)1);
}

void java_lang_System_registerNatives() {
    registerNativeMethod("java/lang/System", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/System", "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V", arraycopy);
    registerNativeMethod("java/lang/System", "initProperties",
                         "(Ljava/util/Properties;)Ljava/util/Properties;", initProperties);

    registerNativeMethod("java/lang/System", "setIn0", "(Ljava/io/InputStream;)V", setIn0);
    registerNativeMethod("java/lang/System", "setOut0", "(Ljava/io/PrintStream;)V", setOut0);
    registerNativeMethod("java/lang/System", "setErr0", "(Ljava/io/PrintStream;)V", setErr0);

    registerNativeMethod("java/lang/System", "nanoTime", "()J", nanoTime);
}
