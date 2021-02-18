#include "../../jni_internal.h"

/**
 * Get an extended exception message. This returns a string describing
 * the location and cause of the exception. It returns null for
 * exceptions where this is not applicable.
 *
 * private native String getExtendedNPEMessage();
 */
static jstring getExtendedNPEMessage()
{
    // todo
    return jnull;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getExtendedNPEMessage", "()" STR, TA(getExtendedNPEMessage) },
};

void java_lang_NullPointerException_registerNatives()
{
    registerNatives("java/lang/NullPointerException", methods, ARRAY_LENGTH(methods));
}
