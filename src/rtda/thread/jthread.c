/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jthread.h"
//#include "../../interpreter/stack_frame.h"

struct jthread* jthread_create()
{
    VM_MALLOC(struct jthread, thread);

    utarray_new(thread->vm_stack, &ut_ptr_icd);

    return thread;
}

void jthread_push_frame(struct jthread *thread, struct stack_frame *frame)
{
    assert(thread != NULL && frame != NULL);
    utarray_push_back(thread->vm_stack, &frame);
}

void jthread_destroy(struct jthread *thread)
{
    // todo
}
