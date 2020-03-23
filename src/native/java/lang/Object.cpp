/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../objects/class.h"
#include "../../../objects/object.h"
#include "../../../symbol.h"
#include "../../../runtime/frame.h"
#include "../../../objects/class_loader.h"

// public native int hashCode();
static void hashCode(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    frame->pushi((jint) (intptr_t) _this);
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    if (!_this->clazz->isSubclassOf(loadBootClass(S(java_lang_Cloneable)))) {
        throw CloneNotSupportedException();
    }
    frame->pushr(_this->clone());
}

// public final native Class<?> getClass();
static void getClass(Frame *frame)
{
    jref _this = frame->getLocalAsRef(0);
    frame->pushr(_this->clazz); // todo 对不对
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
    registerNative("java/lang/Object", "hashCode", "()I", hashCode);
    registerNative("java/lang/Object", "getClass", "()Ljava/lang/Class;", getClass);
    registerNative("java/lang/Object", "clone", "()Ljava/lang/Object;", clone);
    registerNative("java/lang/Object", "notifyAll", "()V", notifyAll);
    registerNative("java/lang/Object", "notify", "()V", notify);
    registerNative("java/lang/Object", "wait", "(J)V", wait);
}
