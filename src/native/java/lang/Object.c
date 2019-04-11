/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"

// public native int hashCode();
static void hashCode(Frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushi(frame, (jint) (intptr_t) this);
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone(Frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    Class *cloneable = load_sys_class("java/lang/Cloneable");
    if (!class_is_subclass_of(this->clazz, cloneable)) {
        jvm_abort("java.lang.CloneNotSupportedException"); // todo
    }
    frame_stack_pushr(frame, object_clone(this));
}

// public final native Class<?> getClass();
static void getClass(Frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushr(frame, this->clazz->clsobj); // todo 对不对
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
    register_native_method("java/lang/Object~registerNatives~()V", registerNatives);
    register_native_method("java/lang/Object~hashCode~()I", hashCode);
    register_native_method("java/lang/Object~getClass~()Ljava/lang/Class;", getClass);
    register_native_method("java/lang/Object~clone~()Ljava/lang/Object;", clone);
    register_native_method("java/lang/Object~notifyAll~()V", notifyAll);
    register_native_method("java/lang/Object~notify~()V", notify);
    register_native_method("java/lang/Object~wait~(J)V", wait);
}
