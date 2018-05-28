/*
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"
#include "../../../rtda/heap/objectarea/JclassObj.h"
#include "../../../rtda/thread/Jthread.h"
#include "../../registry.h"

//// public static native Class<?> getCallerClass(int level)
//static void getCallerClass(StackFrame *frame) {
//    // todo
//}

// public static native Class<?> getCallerClass()
static void getCallerClass(StackFrame *frame) {
    // todo
//    JclassObj *co = frame->method->jclass->getClassObj();
//    frame->operandStack.push(co);



    // top0 is sun/reflect/Reflection
    // top1 is the caller of getCallerClass()
    // top2 is the caller of method
//    callerFrame := frame.Thread().TopFrameN(2)
//    callerClass := callerFrame.Method().Class().JClass()
//    frame.OperandStack().PushRef(callerClass)
    auto &thread = frame->thread;
    auto top0 = thread->topFrame();
    thread->popFrame();
    auto top1 = thread->topFrame();
    thread->popFrame();
    auto callerFrame = thread->topFrame();
    frame->operandStack.push(callerFrame->method->jclass->getClassObj());

    thread->pushFrame(top1);
    thread->pushFrame(top0);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(StackFrame *frame) {
    JclassObj *type = static_cast<JclassObj *>(frame->getLocalVars(0)->getReference());
    jint flags = type->getClass()->accessFlags;
    frame->operandStack.push(flags);
}

void sun_reflect_Reflection_registerNatives() {
    registerNativeMethod("sun/reflect/Reflection", "registerNatives", "()V", [](StackFrame *){});
//    registerNativeMethod("sun/reflect/Reflection", "getCallerClass", "(I)Ljava/lang/Class;", getCallerClass);
    registerNativeMethod("sun/reflect/Reflection", "getCallerClass", "()Ljava/lang/Class;", getCallerClass);
    registerNativeMethod("sun/reflect/Reflection", "getClassAccessFlags", "(Ljava/lang/Class;)I", getClassAccessFlags);
}