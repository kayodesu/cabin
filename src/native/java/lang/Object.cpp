/*
 * Author: kayo
 */

#include "../../../objects/class.h"
#include "../../../objects/object.h"
#include "../../../symbol.h"
#include "../../../runtime/frame.h"
#include "../../../objects/class_loader.h"
#include "../../jni_inner.h"

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

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "hashCode", "()I", (void *) hashCode },
        { "getClass", "()Ljava/lang/Class;", (void *) getClass },
        { "clone", "()Ljava/lang/Object;", (void *) clone },
        { "notifyAll", "()V", (void *) notifyAll },
        { "notify", "()V", (void *) notify },
        { "wait", "(J)V", (void *) wait },
};

void java_lang_Object_registerNatives()
{
    registerNatives("java/lang/Object", methods, ARRAY_LENGTH(methods));
}
