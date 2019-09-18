/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/Object.h"
#include "../../../kayo.h"
#include "../../../rtda/thread/Frame.h"
#include "../../../rtda/ma/Class.h"


// public static native Class<?> getCallerClass(int level)
static void getCallerClass0(Frame *frame)
{
    jvm_abort("getCallerClass0");
}

// public static native Class<?> getCallerClass()
static void getCallerClass(Frame *frame)
{
    // top0, current frame is executing getCallerClass()
    // top1, who called getCallerClass, the one who wants to know his caller.
    // top2, the caller of top1, the result.
//    Frame *top0 = NULL;
    Frame *top1 = frame->prev;
    if (top1 == nullptr) {
        raiseException(UNKNOWN_ERROR, ""); // todo
    }

    Frame *top2 = top1->prev;
    if (top2 == nullptr) {
        raiseException(UNKNOWN_ERROR, ""); // todo
    }

    jref o = top2->method->clazz;
    frame->pushr(o);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(Frame *frame)
{
    Object *type = frame->getLocalAsRef(0);
    frame->pushi(type->clazz->accessFlags);
}

void sun_reflect_Reflection_registerNatives()
{
    register_native_method("sun/reflect/Reflection", "getCallerClass", "()Ljava/lang/Class;", getCallerClass);
    register_native_method("sun/reflect/Reflection", "getCallerClass", "(I)Ljava/lang/Class;", getCallerClass0);
    register_native_method("sun/reflect/Reflection", "getClassAccessFlags", "(Ljava/lang/Class;)I", getClassAccessFlags);
}