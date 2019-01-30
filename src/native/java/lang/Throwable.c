/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/heap/strobj.h"
#include "../../../rtda/thread/thread.h"


struct stack_trace {
    int count;
    jref eles[]; // objects of StackTraceElement
};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushr(frame, this);

    int num = vm_stack_depth();
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
    struct frame *f = frame->prev->prev;
//    num -= 2; // 减去执行fillInStackTrace(int) 和 fillInStackTrace() 方法的frame

    for (struct class *c = this->clazz; c != NULL; c = c->super_class) {
        f = f->prev; // jump 执行异常类的构造函数的frame
        if (strcmp(c->class_name, "java/lang/Throwable") == 0) {
            break; // 可以了，遍历到 Throwable 就行了，因为现在在执行 Throwable 的 fillInStackTrace 方法。
        }
    }

    struct stack_trace *trace = vm_malloc(sizeof(struct stack_trace) + sizeof(jref) * num);
    trace->count = 0;

    struct class *c = load_sys_class("java/lang/StackTraceElement");
    for (; f != NULL; f = f->prev) {
        struct object *o = object_create(c);
        trace->eles[trace->count++] = o;

        // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber)
        // may be should call <init>, but 直接赋值 is also ok. todo

        jref file_name = strobj_create(f->method->clazz->source_file_name);
        jref class_name = strobj_create(f->method->clazz->class_name);
        jref method_name = strobj_create(f->method->name);
        int line_number = method_get_line_number(f->method, f->reader.pc - 1); // todo why 减1？ 减去opcode的长度

        set_instance_field_value(o, class_lookup_field(c, "fileName", "Ljava/lang/String;"), (const slot_t *) &file_name);
        set_instance_field_value(o, class_lookup_field(c, "declaringClass", "Ljava/lang/String;"), (const slot_t *) &class_name);
        set_instance_field_value(o, class_lookup_field(c, "methodName", "Ljava/lang/String;"), (const slot_t *) &method_name);
        set_instance_field_value(o, class_lookup_field(c, "lineNumber", "I"), (const slot_t *) &line_number);
    }
    this->u.stack_trace = trace;
}

// native StackTraceElement getStackTraceElement(int index);
static void getStackTraceElement(struct frame *frame)
{
    jref throwable = frame_locals_getr(frame, 0);
    jint index = frame_locals_geti(frame, 1);

    struct stack_trace *trace = throwable->u.stack_trace;
    assert(trace != NULL);
    frame_stack_pushr(frame, trace->eles[index]);
}

// native int getStackTraceDepth();
static void getStackTraceDepth(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    struct stack_trace *trace = this->u.stack_trace;
    assert(trace != NULL);
    frame_stack_pushi(frame, trace->count);
}

void java_lang_Throwable_registerNatives()
{
#undef C
#define C "java/lang/Throwable~"
    register_native_method(C"registerNatives~()V", registerNatives);
    register_native_method(C"fillInStackTrace~(I)Ljava/lang/Throwable;", fillInStackTrace);
    register_native_method(C"getStackTraceElement~(I)Ljava/lang/StackTraceElement;", getStackTraceElement);
    register_native_method(C"getStackTraceDepth~()I", getStackTraceDepth);
}
