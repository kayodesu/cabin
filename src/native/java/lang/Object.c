/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"

// public native int hashCode();
static void hashCode(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushi(frame, (jint) (intptr_t) this);
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    struct class *cloneable = classloader_load_class(frame->m.method->jclass->loader, "java/lang/Cloneable");
    if (!jclass_is_subclass_of(this->jclass, cloneable)) {
        jvm_abort("java.lang.CloneNotSupportedException"); // todo
    }
    frame_stack_pushr(frame, jobject_clone(this, NULL));
}

// public final native Class<?> getClass();
static void getClass(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    frame_stack_pushr(frame, (jref) this->jclass->clsobj); // todo 对不对
}

// public final native void notifyAll();
static void notifyAll(struct frame *frame)
{
//    jref this = frame_locals_getr(frame, 0);
    // todo
}

// public final native void notify();
static void notify(struct frame *frame)
{
//    jref this = frame_locals_getr(frame, 0);
    // todo
}

// public final native void wait(long timeout) throws InterruptedException;
static void wait(struct frame *frame)
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
