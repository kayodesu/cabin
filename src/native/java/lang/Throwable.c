/*
 * Author: Jia Yang
 */

#include "../../registry.h"

//struct StackTraceElement {
//    string fileName;
//    string className;
//    string methodName;
//    int lineNumber;
//};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(struct stack_frame *frame)
{
    jvm_abort("error\n");
    // todo
    printvm("error  fillInStackTrace.................................................................................\n");
//    JObject *throwable = frame->getLocalVar(0).getRef();
//    // todo
//    frame->operandStack.push(throwable);
}

void java_lang_Throwable_registerNatives()
{
    register_native_method("java/lang/Throwable", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/Throwable", "fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace);
}