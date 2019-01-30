/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/object.h"
#include "../../../rtda/thread/frame.h"


// public static native Class<?> getCallerClass(int level)
static void getCallerClass0(struct frame *frame)
{
    jvm_abort("");
}

// public static native Class<?> getCallerClass()
static void getCallerClass(struct frame *frame)
{
    // top0, current frame is executing getCallerClass()
    // top1, who called getCallerClass, the one who wants to know his caller.
    // top2, the caller of top1, the result.
//    struct frame *top0 = NULL;
    struct frame *top1 = frame->prev;
    if (top1 == NULL) {
        VM_UNKNOWN_ERROR(""); // todo
        return;
    }
    struct frame *top2 = top1->prev;

//    int depth = thread_stack_depth(frame->thread);
//    for (int i = 0; i < depth; i++) {
//        // 过滤掉 shim frame
//        struct frame *sf = thread_stack_frame_from_top(frame->thread, i);
////        if (sf->type == SF_TYPE_NORMAL) {
//            if (top0 == NULL) {
//                top0 = sf;
//            } else if (top1 == NULL) {
//                top1 = sf;
//            } else {
//                top2 = sf;
//                break;
//            }
////        }
//    }
    if (top2 == NULL) {
        VM_UNKNOWN_ERROR(""); // todo
        return;
    }

    jref o = top2->method->clazz->clsobj;
    frame_stack_pushr(frame, o);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(struct frame *frame)
{
    struct object *type = frame_locals_getr(frame, 0);
    frame_stack_pushi(frame, type->clazz->access_flags);
}

void sun_reflect_Reflection_registerNatives()
{
    register_native_method("sun/reflect/Reflection~registerNatives~()V", registerNatives);
    register_native_method("sun/reflect/Reflection~getCallerClass~()Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection~getCallerClass~(I)Ljava/lang/Class;", getCallerClass0);
    register_native_method("sun/reflect/Reflection~getClassAccessFlags~(Ljava/lang/Class;)I", getClassAccessFlags);
}