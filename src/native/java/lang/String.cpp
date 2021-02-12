#include "../../jni_internal.h"
#include "../../../metadata/class.h"

// public native String intern();
static jstring intern(jstring _this)
{
    assert(g_string_class == _this->clazz);
    return _this->clazz->intern(_this);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "intern", "()Ljava/lang/String;", TA(intern) },
};

void java_lang_String_registerNatives()
{
    registerNatives("java/lang/String", methods, ARRAY_LENGTH(methods));
}
