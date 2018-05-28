/*
 * 实现 java/lang/String 类下的 native方法
 *
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/objectarea/JstringObj.h"
#include "../../../rtda/heap/objectarea/strpool.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"


// public native String intern();
static void intern(StackFrame *frame) {
    // todo
    auto thisStr = static_cast<JstringObj *>(frame->getLocalVars(0)->getReference());
    JstringObj *inPoolStr = putStrToPool(frame->method->jclass->loader, thisStr->value());
    frame->operandStack.push(inPoolStr);
}

void java_lang_String_registerNatives() {
    registerNativeMethod("java/lang/String", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}