/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../rtda/ma/Class.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/heap/ClassObject.h"
#include "../../../symbol.h"

// public native int hashCode();
static void hashCode(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    frame->pushi((jint) (intptr_t) thisObj);
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    Class *cloneable = java_lang_Cloneable_class;
    if (!thisObj->clazz->isSubclassOf(cloneable)) {
        jvm_abort("java.lang.CloneNotSupportedException"); // todo
    }
    frame->pushr(thisObj->clone());
}

// public final native Class<?> getClass();
static void getClass(Frame *frame)
{
    jref thisObj = frame->getLocalAsRef(0);
    frame->pushr(thisObj->clazz->clsobj); // todo 对不对
}

// public final native void notifyAll();
static void notifyAll(Frame *frame)
{
//    jref this = frame_locals_getr(frame, 0);
    // todo
}

// public final native void notify();
static void notify(Frame *frame)
{
//    jref this = frame_locals_getr(frame, 0);
    // todo
}

// public final native void wait(long timeout) throws InterruptedException;
static void wait(Frame *frame)
{
//    jref this = frame_locals_getr(frame, 0);
//    jlong timeout = slot_getl(frame->local_vars + 1);
    // todo
}

void java_lang_Object_registerNatives()
{
    register_native_method("java/lang/Object", "hashCode", "()I", hashCode);
    register_native_method("java/lang/Object", "getClass", "()Ljava/lang/Class;", getClass);
    register_native_method("java/lang/Object", "clone", "()Ljava/lang/Object;", clone);
    register_native_method("java/lang/Object", "notifyAll", "()V", notifyAll);
    register_native_method("java/lang/Object", "notify", "()V", notify);
    register_native_method("java/lang/Object", "wait", "(J)V", wait);
}
