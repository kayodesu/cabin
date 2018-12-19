/*
 * Author: Jia Yang
 */

#ifndef JVM_RESERVED_H
#define JVM_RESERVED_H

#include "../interpreter/stack_frame.h"
#include "../native/registry.h"
#include "../rtda/ma/access.h"

// 没有使用的指令
static void notused(struct stack_frame *frame)
{
    jvm_abort("This instruction isn't used. %s\n", sf_to_string(frame));
}

static void breakpoint(struct stack_frame *frame)
{
    // todo
    jvm_abort("debugger used instructions. %s\n", sf_to_string(frame));
}

// todo 此方法感觉实现的有毛病
static void invoke_native_method(struct stack_frame *frame)
{
    const char *class_name = frame->method->jclass->class_name;
    const char *method_name = frame->method->name;
    const char *method_descriptor = frame->method->descriptor;

    assert(IS_NATIVE(frame->method->access_flags));
    find_native_method(class_name, method_name, method_descriptor)(frame);
}

static void impdep2(struct stack_frame *frame)
{
    jvm_abort("jvm used instruction, not used in this jvm. %s", sf_to_string(frame));
}

#endif //JVM_RESERVED_H
