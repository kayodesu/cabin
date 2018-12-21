/*
 * Author: Jia Yang
 */

#include <stdio.h>
#include <assert.h>
#include "vm_error.h"
#include "jvm.h"

void vm_internal_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}

void vm_out_of_memory_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}

void vm_stack_overflow_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}

void vm_unknown_error(const char *msg)
{
    assert(msg != NULL);
    // todo
    jvm_abort(msg);
}
