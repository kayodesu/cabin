/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../cli.h"

// public native String intern();
static jstring intern(JNIEnv *env, jstring _this)
{
    return cli_intern(_this);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "intern", "()Ljava/lang/String;", (void *) intern },
};

void java_lang_String_registerNatives()
{
    registerNatives("java/lang/String", methods, ARRAY_LENGTH(methods));
}
