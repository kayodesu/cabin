/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../util/endianness.h"

// private static native boolean isBigEndian();
static jbool _isBigEndian()
{
    return isBigEndian() ? jtrue : jfalse;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "isBigEndian", "()B", (void *) _isBigEndian },
};

void java_lang_StringUTF16_registerNatives()
{
    registerNatives("java/lang/StringUTF16", methods, ARRAY_LENGTH(methods));
}
