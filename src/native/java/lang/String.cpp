/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/StrPool.h"

// todo 这函数是干嘛的
// public native String intern();
static void intern(Frame *frame)
{
    // todo
    auto thisObj = frame->getLocalAsRef<StringObject>(0);

//    put_so_to_pool(this);
    auto soInPool = g_str_pool.put(thisObj);
    frame_stack_pushr(frame, soInPool);
}

void java_lang_String_registerNatives()
{
    register_native_method("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
