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
    // top0 is sun/reflect/Reflection
    // top1 is the caller of getCallerClass()
    // top2 is the caller of method
    struct stack_frame *top0 = NULL;
    struct stack_frame *top1 = NULL;
    struct stack_frame *top2 = NULL;

    int depth = jthread_stack_depth(frame->thread);
    for (int i = 0; i < depth; i++) {
        struct stack_frame *sf = jthread_depth_frame(frame->thread, i);
        if (sf->type == SF_TYPE_NORMAL) {
            if (top0 == NULL) {
                top0 = sf;
            } else if (top1 == NULL) {
                top1 = sf;
            } else {
                top2 = sf;
                break;
            }
        }
    }
    if (top2 == NULL) {
        VM_UNKNOWN_ERROR(""); // todo
        return;
    }

    os_pushr(frame->operand_stack, top2->method->jclass->clsobj);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(struct stack_frame *frame)
{
    struct jobject *type = slot_getr(frame->local_vars);
#ifdef JVM_DEBUG
    JOBJECT_CHECK_CLSOBJ(type);
#endif
    os_pushi(frame->operand_stack, type->jclass->access_flags);
}

void sun_reflect_Reflection_registerNatives()
{
    register_native_method("sun/reflect/Reflection~registerNatives~()V", registerNatives);
    register_native_method("sun/reflect/Reflection~getCallerClass~()Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection~getClassAccessFlags~(Ljava/lang/Class;)I", getClassAccessFlags);
}