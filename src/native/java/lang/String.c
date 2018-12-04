/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/strpool.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../rtda/heap/jobject.h"

// todo 这函数是干嘛的
// public native String intern();
static void intern(struct stack_frame *frame)
{
    // todo
    jref so = slot_getr(frame->local_vars);
#ifdef JVM_DEBUG
    JOBJECT_CHECK_STROBJ(so);
#endif
    put_so_to_pool(so);
    os_pushr(frame->operand_stack, so);
}

void java_lang_String_registerNatives()
{
    register_native_method("java/lang/String", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
