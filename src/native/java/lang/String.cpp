/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../rtda/heap/StrPool.h"

// todo 这函数是干嘛的
// public native String intern();
static void intern(Frame *frame)
{
    // todo
    auto _this = frame->getLocalAsRef<StringObject>(0);

//    put_so_to_pool(this);
    auto soInPool = vmEnv.strPool->put(_this);
    frame->pushr(soInPool);
}

void java_lang_String_registerNatives()
{
    register_native_method("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
