/*
 * Author: Yo Ka
 */

#include "../../../runtime/frame.h"
#include "../../../objects/class.h"
#include "../../jni_inner.h"

// public native String intern();
static jstrref intern(JNIEnv *env, jref _this)
{
    return _this->clazz->intern(_this);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "intern", "()Ljava/lang/String;", (void *) intern },
};

void java_lang_String_registerNatives()
{
    registerNatives("java/lang/String", methods, ARRAY_LENGTH(methods));
}
