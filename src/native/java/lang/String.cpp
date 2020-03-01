/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../runtime/Frame.h"
#include "../../../objects/class.h"

// public native String intern();
static void intern(Frame *frame)
{
    auto _this = frame->getLocalAsRef(0);
    frame->pushr(_this->clazz->intern(_this));
}

void java_lang_String_registerNatives()
{
    registerNative("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
