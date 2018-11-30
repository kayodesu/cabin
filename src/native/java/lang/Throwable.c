/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"

//struct StackTraceElement {
//    string fileName;
//    string className;
//    string methodName;
//    int lineNumber;
//};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(struct stack_frame *frame)
{
    jvm_abort("error");
    // todo
//    JObject *throwable = frame->getLocalVar(0).getRef();
//    // todo
//    frame->operandStack.push(throwable);

//    struct jobject *throwable = slot_getr(frame->local_vars);
//    os_pushr(frame->operand_stack, throwable);
}

void java_lang_Throwable_registerNatives()
{
    register_native_method("java/lang/Throwable", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/Throwable", "fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace);
}