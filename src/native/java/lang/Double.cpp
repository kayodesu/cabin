/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../slot.h"
#include "../../../rtda/thread/Frame.h"

// public static native long doubleToRawLongBits(double value);
static void doubleToRawLongBits(Frame *frame)
{
    jdouble d = frame_locals_getd(frame, 0);
    frame_stack_pushl(frame, double_to_raw_long_bits(d));
}

// public static native double longBitsToDouble(long value);
static void longBitsToDouble(Frame *frame)
{
    jlong l = frame_locals_getl(frame, 0);
    frame_stack_pushd(frame, long_bits_to_double(l));
}

void java_lang_Double_registerNatives()
{
    register_native_method("java/lang/Double", "doubleToRawLongBits", "(D)J", doubleToRawLongBits);
    register_native_method("java/lang/Double", "longBitsToDouble", "(J)D", longBitsToDouble);
}
