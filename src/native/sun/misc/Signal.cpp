#include "../../jni_inner.h"
#include "../../../runtime/frame.h"

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

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "findSignal", "(Ljava/lang/String;)I", (void *) findSignal },
        { "handle0", "(IJ)J", (void *) handle0 },
        { "raise0", "(I)V", (void *) raise0 },
};

void sun_misc_Signal_registerNatives()
{
    registerNatives("sun/misc/Signal", methods, ARRAY_LENGTH(methods));
}
