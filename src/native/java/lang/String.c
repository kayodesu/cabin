/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// public native String intern();
static void intern(struct stack_frame *frame)
{
    // todo
//    auto thisStr = dynamic_cast<JStringObj *>(frame->getLocalVar(0).getRef());
//    JStringObj *inPoolStr = putStrToPool(frame->method->jclass->loader, thisStr->value());
//    frame->operandStack.push(inPoolStr);
}

void java_lang_String_registerNatives(struct stack_frame *frame)
{
    register_native_method("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
