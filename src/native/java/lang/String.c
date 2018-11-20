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
    jref so = slot_getr(frame->local_vars); // frame->local_vars[0]
    STROBJ_CHECK(so);
    put_so_to_pool(frame->method->jclass->loader, so);
    os_pushr(frame->operand_stack, so);

//    auto thisStr = dynamic_cast<JStringObj *>(frame->getLocalVar(0).getRef());
//    JStringObj *inPoolStr = putStrToPool(frame->method->jclass->loader, thisStr->value());
//    frame->operandStack.push(inPoolStr);
}

void java_lang_String_registerNatives()
{
    register_native_method("java/lang/String", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/String", "intern", "()Ljava/lang/String;", intern);
}
