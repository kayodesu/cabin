/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../util/convert.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"

// public static native long doubleToRawLongBits(double value);
static void doubleToRawLongBits(struct stack_frame *frame)
{
    jdouble d = slot_getd(frame->local_vars);
    os_pushl(frame->operand_stack, double_to_raw_long_bits(d));
}

// public static native double longBitsToDouble(long value);
static void longBitsToDouble(struct stack_frame *frame)
{
    jlong l = slot_getl(frame->local_vars);
    os_pushd(frame->operand_stack, long_bits_to_double(l));
}

void java_lang_Double_registerNatives()
{
    register_native_method("java/lang/Double", "registerNatives", "()V", registerNatives);
    register_native_method("java/lang/Double", "doubleToRawLongBits", "(D)J", doubleToRawLongBits);
    register_native_method("java/lang/Double", "longBitsToDouble", "(J)D", longBitsToDouble);
}
