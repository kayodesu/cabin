/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/jobject.h"

//// public static native Class<?> getCallerClass(int level)
//static void getCallerClass(StackFrame *frame) {
//    // todo
//}

// public static native Class<?> getCallerClass()
static void getCallerClass(struct stack_frame *frame)
{
    // todo
//    JClassObj *co = frame->method->jclass->getClassObj();
//    frame->operandStack.push(co);

    // top0 is sun/reflect/Reflection
    // top1 is the caller of getCallerClass()
    // top2 is the caller of method
//    callerFrame := frame.Thread().TopFrameN(2)
//    callerClass := callerFrame.Method().Class().JClass()
//    frame.OperandStack().PushRef(callerClass)

    // todo
    // top0 is sun/reflect/Reflection
    // top1 is the caller of getCallerClass()
    // top2 is the caller of method
    struct stack_frame *top0 = jthread_top_frame(frame->thread);
    jthread_pop_frame(frame->thread);
    struct stack_frame *top1 = jthread_top_frame(frame->thread);
    jthread_pop_frame(frame->thread);
    struct stack_frame *top2 = jthread_top_frame(frame->thread);
    os_pushr(frame->operand_stack, top2->method->jclass->clsobj);
    jthread_push_frame(frame->thread, top1);
    jthread_push_frame(frame->thread, top0);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(struct stack_frame *frame)
{
    struct jobject *type = slot_getr(frame->local_vars);
    // todo 判断是不是 class object
    os_pushi(frame->operand_stack, type->jclass->access_flags);
}

void sun_reflect_Reflection_registerNatives()
{
    register_native_method("sun/reflect/Reflection", "registerNatives", "()V", registerNatives);
//    register_native_method("sun/reflect/Reflection", "getCallerClass", "(I)Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection", "getCallerClass", "()Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection", "getClassAccessFlags", "(Ljava/lang/Class;)I", getClassAccessFlags);
}