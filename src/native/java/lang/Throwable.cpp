/*
 * Author: Jia Yang
 */

#include <string>
#include "../../registry.h"

using namespace std;

struct StackTraceElement {
    string fileName;
    string className;
    string methodName;
    int lineNumber;
};

// private native Throwable fillInStackTrace(int dummy);
static void fillInStackTrace(StackFrame *frame) {
    // todo
    jprintf("error  fillInStackTrace.................................................................................\n");
    Jobject *throwable = static_cast<Jobject *>(frame->getLocalVars(0)->getReference());
    // todo
    frame->operandStack.push(throwable);
}

void java_lang_Throwable_registerNatives() {
    registerNativeMethod("java/lang/Throwable", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Throwable", "fillInStackTrace", "(I)Ljava/lang/Throwable;", fillInStackTrace);
}