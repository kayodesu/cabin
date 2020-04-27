/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../../vmdef.h"
#include "../../../objects/class.h"
#include "../../../objects/string_object.h"

// public native String intern();
static jstrref intern(jstrref _this)
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
