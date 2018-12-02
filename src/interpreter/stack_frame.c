/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "stack_frame.h"
#include "../slot.h"

struct stack_frame* sf_create_shim(struct jthread *thread)
{
    assert(thread != NULL);

    VM_MALLOC(struct stack_frame, frame);

    frame->thread = thread;
    frame->method = NULL;
    frame->operand_stack = os_create(8); // 8 is big enough, 只是用来接收函数返回值
    frame->max_locals = NULL;
    frame->local_vars = NULL;
    frame->reader = NULL;
    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;

    return frame;
}

struct stack_frame* sf_create(struct jthread *thread, struct jmethod *method)
{
    assert(thread != NULL);
    assert(method != NULL);

    VM_MALLOC(struct stack_frame, frame);

    frame->thread = thread;
    frame->method = method;
    frame->operand_stack = os_create(method->max_stack);
    frame->max_locals = method->max_locals;
    frame->local_vars = malloc(sizeof(struct slot) * frame->max_locals);
    frame->reader = bcr_create(method->code, method->code_length);
    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;

    return frame;
}

void sf_set_local_var(struct stack_frame *frame, int index, const struct slot *value)
{
    assert(frame != NULL);
    assert(index >= 0 && index < frame->max_locals);

    frame->local_vars[index] = *value;
    if (slot_is_category_two(value)) {
        frame->local_vars[++index] = phslot;
    }
}

void sf_destroy(struct stack_frame *frame)
{
    if (frame == NULL)
        return;

    free(frame->local_vars);
    bcr_destroy(frame->reader);
    os_destroy(frame->operand_stack);

    // todo
//    free(frame);
}

char* sf_to_string(const struct stack_frame *frame)
{
    return jmethod_to_string(frame->method); // todo
}
