/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/strpool.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/jobject.h"

// todo 这函数是干嘛的
// public native String intern();
static void intern(struct frame *frame)
{
    // todo
    jref this = frame_locals_getr(frame, 0);

    put_so_to_pool(this);
    frame_stack_pushr(frame, this);
}

void java_lang_String_registerNatives()
{
    register_native_method("java/lang/String~registerNatives~()V", registerNatives);
    register_native_method("java/lang/String~intern~()Ljava/lang/String;", intern);
}
