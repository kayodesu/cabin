/*
 * Author: Jia Yang
 */

#ifndef JVM_RESERVED_H
#define JVM_RESERVED_H

#include "../interpreter/stack_frame.h"
#include "../native/registry.h"

// 没有使用的指令
static void notused(struct stack_frame *frame)
{
    jvm_abort("This instruction isn't used.\n");
}

static void breakpoint(struct stack_frame *frame)
{
    // todo
    jvm_abort("debugger used instructions.\n");
}

// todo 此方法感觉实现的有毛病
static void invoke_native_method(struct stack_frame *frame)
{
    const char *class_name = frame->method->jclass->class_name;
    const char *method_name = frame->method->name;
    const char *method_descriptor = frame->method->descriptor;

    printvm("invoke native method: %s~%s~%s ----------------\n", class_name, method_name, method_descriptor);

    native_method_f native_method = find_native_method(class_name, method_name, method_descriptor);
    if (native_method == NULL) {
        // todo
        jvm_abort("don't find native method：%s~%s~%s\n", class_name, method_name, method_descriptor);
    }
    native_method(frame);
}

#endif //JVM_RESERVED_H
