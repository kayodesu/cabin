/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../objects/object.h"
#include "../../../kayo.h"
#include "../../../runtime/frame.h"
#include "../../../objects/class.h"


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
    Frame *top1 = frame->prev;
    assert(top1 != nullptr);

    Frame *top2 = top1->prev;
    assert(top2 != nullptr);

    jref o = top2->method->clazz;
    frame->pushr(o);
}

// public static native int getClassAccessFlags(Class<?> type)
static void getClassAccessFlags(Frame *frame)
{
    Object *type = frame->getLocalAsRef(0);
    frame->pushi(type->clazz->modifiers);
}

void sun_reflect_Reflection_registerNatives()
{
    registerNative("sun/reflect/Reflection", "getCallerClass", "()Ljava/lang/Class;", getCallerClass);
    registerNative("sun/reflect/Reflection", "getCallerClass", "(I)Ljava/lang/Class;", getCallerClass0);
    registerNative("sun/reflect/Reflection", "getClassAccessFlags", "(Ljava/lang/Class;)I", getClassAccessFlags);
}