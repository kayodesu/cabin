/*
 * Author: kayo
 */

#include "../../../runtime/frame.h"
#include "../../../objects/class.h"
#include "../../jni_inner.h"

// public native String intern();
static void intern(Frame *frame)
{
    auto _this = frame->getLocalAsRef(0);
    frame->pushr(_this->clazz->intern(_this));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "intern", "()Ljava/lang/String;", (void *) intern },
};

void java_lang_String_registerNatives()
{
    registerNatives("java/lang/String", methods, ARRAY_LENGTH(methods));
}
