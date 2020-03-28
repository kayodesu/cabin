/*
 * Author: kayo
 */

#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

// public static native long doubleToRawLongBits(double value);
static void doubleToRawLongBits(Frame *frame)
{
    jdouble d = frame->getLocalAsDouble(0);
    frame->pushl(double_to_raw_long_bits(d));
}

// public static native double longBitsToDouble(long value);
static void longBitsToDouble(Frame *frame)
{
    jlong l = frame->getLocalAsLong(0);
    frame->pushd(long_bits_to_double(l));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "doubleToRawLongBits", "(D)J", (void *) doubleToRawLongBits },
        { "longBitsToDouble", "(J)D", (void *) longBitsToDouble }
};

void java_lang_Double_registerNatives()
{
    registerNatives("java/lang/Double", methods, ARRAY_LENGTH(methods));
}
