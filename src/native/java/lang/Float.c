/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../util/convert.h"

// public static native int floatToRawIntBits(float value);
static void floatToRawIntBits(struct stack_frame *frame)
{
    jfloat f = slot_getf(frame->local_vars); // frame->local_vars[0]
    os_pushi(frame->operand_stack, float_to_raw_int_bits(f));
}

// public static native float intBitsToFloat(int value);
static void intBitsToFloat(struct stack_frame *frame)
{
    jint i = slot_geti(frame->local_vars); // frame->local_vars[0]
    os_pushf(frame->operand_stack, int_bits_to_float(i));
}

void java_lang_Float_registerNatives(struct stack_frame *frame)
{
    register_native_method("java/lang/Float", "floatToRawIntBits", "(F)I", floatToRawIntBits);
    register_native_method("java/lang/Float", "intBitsToFloat", "(I)F", intBitsToFloat);
}