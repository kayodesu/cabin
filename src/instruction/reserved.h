/*
 * Author: Jia Yang
 */

#ifndef JVM_RESERVED_H
#define JVM_RESERVED_H

#include "../rtda/thread/frame.h"
#include "../native/registry.h"
#include "../rtda/ma/access.h"

#if 0
// 没有使用的指令
static void notused(struct frame *frame)
{
    jvm_abort("This instruction isn't used. %s\n", frame_to_string(frame));
}

static void breakpoint(struct frame *frame)
{
    // todo
    jvm_abort("debugger used instructions. %s\n", frame_to_string(frame));
}

// todo 此方法感觉实现的有毛病
static void invoke_native_method(struct frame *frame)
{
    assert(IS_NATIVE(frame->m.method->access_flags));
    assert(frame->m.method->native_method != NULL);
    frame->m.method->native_method(frame);
}

static void impdep2(struct frame *frame)
{
    jvm_abort("jvm used instruction, not used in this jvm. %s", frame_to_string(frame));
}
#endif

#endif //JVM_RESERVED_H
