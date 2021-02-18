#include "../../jni_internal.h"
#include "../../../util/endianness.h"

// private static native boolean isBigEndian();
static jbool isBigEndian0()
{
    return isBigEndian() ? jtrue : jfalse;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "isBigEndian", "()Z", TA(isBigEndian0) },
};

void java_lang_StringUTF16_registerNatives()
{
    registerNatives("java/lang/StringUTF16", methods, ARRAY_LENGTH(methods));
}
