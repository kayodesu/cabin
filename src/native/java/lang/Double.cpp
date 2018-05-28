/*
 * 实现 java/lang/Double 类下的 native方法
 *
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"

// public static native long doubleToRawLongBits(double value);
static void doubleToRawLongBits(StackFrame *frame) {
    jdouble d = frame->getLocalVars(0)->getDouble();
    frame->operandStack.push(jvm::convert::doubleToRawLongBits(d));
}

// public static native double longBitsToDouble(long value);
static void longBitsToDouble(StackFrame *frame) {
    jlong l = frame->getLocalVars(0)->getLong();
    frame->operandStack.push(jvm::convert::longBitsToDouble(l));
}

void java_lang_Double_registerNatives() {
    registerNativeMethod("java/lang/Double", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Double", "doubleToRawLongBits", "(D)J", doubleToRawLongBits);
    registerNativeMethod("java/lang/Double", "longBitsToDouble", "(J)D", longBitsToDouble);
}
