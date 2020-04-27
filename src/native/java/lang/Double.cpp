/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../../util/convert.h"
#include "../../../vmdef.h"

// public static native long doubleToRawLongBits(double value);
static jlong doubleToRawLongBits(jclsref clazz, jdouble value)
{
    return double_to_raw_long_bits(value);
}

// public static native double longBitsToDouble(long value);
static jdouble longBitsToDouble(jclsref clazz, jlong value)
{
    return long_bits_to_double(value);
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
