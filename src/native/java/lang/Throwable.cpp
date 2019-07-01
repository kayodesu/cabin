/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/heap/StringObject.h"
#include "../../../rtda/thread/Thread.h"
#include "../../../utf8.h"
#include "../../../symbol.h"

using namespace std;

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    frame_stack_pushr(frame, thisObj);

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
    Frame *f = frame->prev->prev;
//    num -= 2; // 减去执行fillInStackTrace(int) 和 fillInStackTrace() 方法的frame

    for (Class *c = thisObj->clazz; c != nullptr; c = c->superClass) {
        f = f->prev; // jump 执行异常类的构造函数的frame
        if (utf8_equals(c->className, S(java_lang_Throwable))) {
            break; // 可以了，遍历到 Throwable 就行了，因为现在在执行 Throwable 的 fillInStackTrace 方法。
        }
    }

    auto trace = new vector<Object *>;

    Class *c = loadSysClass(S(java_lang_StackTraceElement));
    for (; f != nullptr; f = f->prev) {
        Object *o = Object::newInst(c);
        trace->push_back(o);

        // public StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber)
        // may be should call <init>, but 直接赋值 is also ok. todo

        auto fileName = StringObject::newInst(f->method->clazz->sourceFileName);
        auto className = StringObject::newInst(f->method->clazz->className);
        auto methodName = StringObject::newInst(f->method->name);
        int lineNumber = f->method->getLineNumber(f->reader.pc - 1); // todo why 减1？ 减去opcode的长度

        o->setInstFieldValue("fileName", "Ljava/lang/String;", (const slot_t *) &fileName);
        o->setInstFieldValue("declaringClass", "Ljava/lang/String;", (const slot_t *) &className);
        o->setInstFieldValue("methodName", "Ljava/lang/String;", (const slot_t *) &methodName);
        o->setInstFieldValue("lineNumber", S(I), (const slot_t *) &lineNumber);
    }

    thisObj->extra = trace;
}

// native StackTraceElement getStackTraceElement(int index);
static void getStackTraceElement(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    jint index = frame->getLocalAsInt(1);

    auto trace = (vector<Object *> *) thisObj->extra;
    assert(trace != nullptr);
    frame_stack_pushr(frame, trace->at(index));
}

// native int getStackTraceDepth();
static void getStackTraceDepth(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    auto trace = (vector<Object *> *) thisObj->extra;
    assert(trace != nullptr);
    frame_stack_pushi(frame, trace->size());
}

void java_lang_Throwable_registerNatives()
{
#undef C
#define C "java/lang/Throwable",
    register_native_method(C"fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace);
    register_native_method(C"getStackTraceElement", "(I)Ljava/lang/StackTraceElement;", getStackTraceElement);
    register_native_method(C"getStackTraceDepth", "()I", getStackTraceDepth);
}
