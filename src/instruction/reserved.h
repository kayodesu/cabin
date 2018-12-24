/*
 * Author: Jia Yang
 */

#ifndef JVM_RESERVED_H
#define JVM_RESERVED_H

#include "../rtda/thread/frame.h"
#include "../native/registry.h"
#include "../rtda/ma/access.h"

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
    const char *class_name = frame->m.method->jclass->class_name;
    const char *method_name = frame->m.method->name;
    const char *method_descriptor = frame->m.method->descriptor;

    assert(IS_NATIVE(frame->m.method->access_flags));
    find_native_method(class_name, method_name, method_descriptor)(frame);

//    assert(frame == jthread_top_frame(frame->thread));
//    jthread_pop_frame(frame->thread);
//    frame_exe_over(frame);
//
//    if (!slot_is_null(&ret_value)) {
//        struct frame *invoke_frame = jthread_top_frame(frame->thread);
//        assert(invoke_frame != NULL);
//        frame_stack_push_slot(invoke_frame, &ret_value); // 将函数返回值传递给调用者
//    }
}

static void impdep2(struct frame *frame)
{
    jvm_abort("jvm used instruction, not used in this jvm. %s", frame_to_string(frame));
}

#endif //JVM_RESERVED_H
