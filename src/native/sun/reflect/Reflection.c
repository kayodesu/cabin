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
static void getCallerClass(struct frame *frame)
{
    // todo
    // top0 is sun/reflect/Reflection
    // top1 is the caller of getCallerClass()
    // top2 is the caller of method
    struct frame *top0 = NULL;
    struct frame *top1 = NULL;
    struct frame *top2 = NULL;

    int depth = jthread_stack_depth(frame->thread);
    for (int i = 0; i < depth; i++) {
        struct frame *sf = jthread_depth_frame(frame->thread, i);
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

    frame_stack_pushr(frame, top2->m.method->jclass->clsobj);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(struct frame *frame)
{
    struct jobject *type = frame_locals_getr(frame, 0);
    frame_stack_pushi(frame, type->jclass->access_flags);
}

void sun_reflect_Reflection_registerNatives()
{
    register_native_method("sun/reflect/Reflection~registerNatives~()V", registerNatives);
    register_native_method("sun/reflect/Reflection~getCallerClass~()Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection~getClassAccessFlags~(Ljava/lang/Class;)I", getClassAccessFlags);
}