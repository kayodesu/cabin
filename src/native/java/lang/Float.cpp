/*
 * 实现 java/lang/Float 类下的 native方法
 *
 * Author: Jia Yang
 */


#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"

// public static native int floatToRawIntBits(float value);
static void floatToRawIntBits(StackFrame *frame) {
    jfloat f = frame->getLocalVars(0)->getFloat();
    frame->operandStack.push(jvm::convert::floatToRawIntBits(f));
}

// public static native float intBitsToFloat(int value);
static void intBitsToFloat(StackFrame *frame) {
    jint i = frame->getLocalVars(0)->getInt();
    frame->operandStack.push(jvm::convert::intBitsToFloat(i));
}

void java_lang_Float_registerNatives() {
    registerNativeMethod("java/lang/Float", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Float", "floatToRawIntBits", "(F)I", floatToRawIntBits);
    registerNativeMethod("java/lang/Float", "intBitsToFloat", "(I)F", intBitsToFloat);
}