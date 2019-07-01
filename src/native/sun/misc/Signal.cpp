#include "../../registry.h"
#include "../../../slot.h"
#include "../../../rtda/heap/Object.h"

/*
 * Author: Jia Yang
 */

// private static native int findSignal(String string);
static void findSignal(Frame *frame)
{
    jref name = frame->getLocalAsRef(0);
    frame->pushi(0); // todo
}

// private static native long handle0(int i, long l);
static void handle0(Frame *frame)
{
//    jint i = slot_geti(frame->local_vars);
//    jlong l = slot_getl(frame->local_vars + 1);

    frame->pushl(0); // todo
}

// private static native void raise0(int i);
static void raise0(Frame *frame)
{
//    jint i = slot_geti(frame->local_vars);
    // todo
}

void sun_misc_Signal_registerNatives()
{
    register_native_method("sun/misc/Signal", "findSignal", "(Ljava/lang/String;)I", findSignal);
    register_native_method("sun/misc/Signal", "handle0", "(IJ)J", handle0);
    register_native_method("sun/misc/Signal", "raise0", "(I)V", raise0);
}
