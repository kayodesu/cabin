/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"


struct stack_trace {
    int count;
    jref eles[]; // objects of StackTraceElement
};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushr(frame, this);

    int num;
    struct frame **frames = jthread_get_frames(frame->thread, &num);
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
    num -= 2; // 减去执行fillInStackTrace(int) 和 fillInStackTrace() 方法的frame
    for (struct jclass *c = this->jclass; c != NULL; c = c->super_class) {
        num--; // 减去执行异常类的构造函数的frame
        if (strcmp(c->class_name, "java/lang/Throwable") == 0) {
            break; // 可以了，遍历到 Throwable 就行了，因为现在在执行 Throwable 的 fillInStackTrace 方法。
        }
    }
    if (num < 0) {
        jvm_abort("Never goes here. num = %d", num);
    }

    VM_MALLOC_EXT(struct stack_trace, 1, sizeof(jref) * num, trace);
    trace->count = 0;

    struct jclass *c = classloader_load_class(g_bootstrap_loader, "java/lang/StackTraceElement");
    for (int i = num - 1; i >= 0; i--) {
        // jump shim frame
        if (frames[i]->type == SF_TYPE_SHIM) {
            continue;
        }

        struct jobject *o = jobject_create(c);
        trace->eles[trace->count++] = o;

        // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber)
        // may be should call <init>, but 直接赋值 is also ok.

        struct slot file_name = rslot(jstrobj_create(frames[i]->m.method->jclass->source_file_name));
        struct slot class_name = rslot(jstrobj_create(frames[i]->m.method->jclass->class_name));
        struct slot method_name = rslot(jstrobj_create(frames[i]->m.method->name));
        struct slot line_number = islot(jmethod_get_line_number(frames[i]->m.method, frames[i]->reader.pc - 1)); // todo why 减1？ 减去opcode的长度

        set_instance_field_value_by_nt(o, "fileName", "Ljava/lang/String;", &file_name);
        set_instance_field_value_by_nt(o, "declaringClass", "Ljava/lang/String;", &class_name);
        set_instance_field_value_by_nt(o, "methodName", "Ljava/lang/String;", &method_name);
        set_instance_field_value_by_nt(o, "lineNumber", "I", &line_number);
    }

    free(frames);
    this->extra = trace;
}

// native StackTraceElement getStackTraceElement(int index);
static void getStackTraceElement(struct frame *frame)
{
    jref throwable = frame_locals_getr(frame, 0);
    jint index = frame_locals_geti(frame, 1);

    struct stack_trace *trace = throwable->extra;
    assert(trace != NULL);
    frame_stack_pushr(frame, trace->eles[index]);
}

// native int getStackTraceDepth();
static void getStackTraceDepth(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    struct stack_trace *trace = this->extra;
    assert(trace != NULL);
    frame_stack_pushi(frame, trace->count);
}

void java_lang_Throwable_registerNatives()
{
    register_native_method("java/lang/Throwable~registerNatives~()V", registerNatives);
    register_native_method("java/lang/Throwable~fillInStackTrace~(I)Ljava/lang/Throwable;", fillInStackTrace);
    register_native_method("java/lang/Throwable~getStackTraceElement~(I)Ljava/lang/StackTraceElement;", getStackTraceElement);
    register_native_method("java/lang/Throwable~getStackTraceDepth~()I", getStackTraceDepth);
}
