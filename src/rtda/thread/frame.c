/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "frame.h"
#include "../ma/access.h"

struct frame* frame_create_shim(struct jthread *thread, void (* shim_action)(struct frame *))
{
    assert(thread != NULL);

    u4 max_locals_stack = 2; // 没有 local vars，操作栈只是用来接收函数返回值，at most 2 slots

    VM_MALLOC_EXT(struct frame, 1, sizeof(struct slot) * max_locals_stack, frame);
    frame->max_locals_and_stack = max_locals_stack;
    frame->stack = frame->locals;
    frame->stack_top = 0;

    frame->type = SF_TYPE_SHIM;
    frame->m.shim_action = shim_action;

    frame->thread = thread;
//    frame->operand_stack = os_create(2); // 2 is big enough, 只是用来接收函数返回值，at most 2 slots
//    frame->max_locals = 0;
//    frame->local_vars = NULL;
//    frame->reader = NULL;
//    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;

    return frame;
}

void frame_bind(struct frame *frame, struct jthread *thread, struct jmethod *method)
{
    assert(frame != NULL);
    assert(thread != NULL);
    assert(method != NULL);

    frame->max_locals_and_stack = method->max_locals + method->max_stack;
    frame->stack = frame->locals + method->max_locals;
    frame->stack_top = 0;

    frame->type = SF_TYPE_NORMAL;
    frame->m.method = method;
    frame->thread = thread;
    bcr_init(&frame->reader, method->code, method->code_length);
    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;
}

struct frame* frame_create_normal(struct jthread *thread, struct jmethod *method)
{
    assert(thread != NULL);
    assert(method != NULL);

    VM_MALLOC_EXT(struct frame, 1, sizeof(struct slot) * (method->max_locals + method->max_stack), frame);
    frame_bind(frame, thread, method);
    return frame;
}

char* frame_to_string(const struct frame *f)
{
    assert(f != NULL);
    int len = strlen(f->m.method->jclass->class_name) + strlen(f->m.method->name) + strlen(f->m.method->descriptor) + 16;
    char *buf = malloc(sizeof(char) * len);
    strcpy(buf, IS_NATIVE(f->m.method->access_flags) ? "(native)" : "");
    strcat(buf, f->m.method->jclass->class_name);
    strcat(buf, "~");
    strcat(buf, f->m.method->name);
    strcat(buf, "~");
    strcat(buf, f->m.method->descriptor);
    return buf; // todo
}

void frame_destroy(struct frame *f)
{
    if (f == NULL)
        return;

    // todo
//    free(frame);
}
