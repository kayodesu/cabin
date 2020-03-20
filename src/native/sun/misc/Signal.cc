#include "../../registry.h"
#include "../../../runtime/Frame.h"

/*
 * Author: kayo
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
#undef C
#define C "sun/misc/Signal"
    registerNative(C, "findSignal", "(Ljava/lang/String;)I", findSignal);
    registerNative(C, "handle0", "(IJ)J", handle0);
    registerNative(C, "raise0", "(I)V", raise0);
}
