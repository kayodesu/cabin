/*
 * Author: kayo
 */

#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

// public static native int floatToRawIntBits(float value);
static void floatToRawIntBits(Frame *frame)
{
    jfloat f = frame->getLocalAsFloat(0);
    frame->pushi(float_to_raw_int_bits(f));
}

// public static native float intBitsToFloat(int value);
static void intBitsToFloat(Frame *frame)
{
    jint i = frame->getLocalAsInt(0);
    frame->pushf(int_bits_to_float(i));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "floatToRawIntBits", "(F)I", (void *) floatToRawIntBits},
        { "intBitsToFloat", "(I)F", (void *) intBitsToFloat }
};

void java_lang_Float_registerNatives()
{
    registerNatives("java/lang/Float", methods, ARRAY_LENGTH(methods));
}