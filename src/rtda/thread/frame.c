/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "frame.h"
#include "../ma/access.h"
#include "../heap/object.h"
#include "../heap/arrobj.h"

struct frame* frame_create_shim(struct thread *thread, void (* shim_action)(struct frame *))
{
    assert(thread != NULL);

    u4 max_locals_stack = 2; // 没有 local vars，操作栈只是用来接收函数返回值，at most 2 slots

    VM_MALLOC_EXT(struct frame, 1, sizeof(struct slot) * max_locals_stack, frame);
    frame->max_locals_and_stack = max_locals_stack;
    frame->stack = frame->locals;
    frame->stack_top = -1;

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

void frame_bind(struct frame *frame, struct thread *thread, struct method *method)
{
    assert(frame != NULL);
    assert(thread != NULL);
    assert(method != NULL);

    frame->max_locals_and_stack = method->max_locals + method->max_stack;
    frame->stack = frame->locals + method->max_locals;
    frame->stack_top = -1;

    frame->type = SF_TYPE_NORMAL;
    frame->m.method = method;
    frame->thread = thread;
    bcr_init(&frame->reader, method->code, method->code_length);
    frame->interrupted_status = frame->exe_status = frame->proc_exception_status = false;
}

struct frame* frame_create_normal(struct thread *thread, struct method *method)
{
    assert(thread != NULL);
    assert(method != NULL);

    VM_MALLOC_EXT(struct frame, 1, sizeof(struct slot) * (method->max_locals + method->max_stack), frame);
    frame_bind(frame, thread, method);
    return frame;
}

#define GET_AND_CHECK_ARRAY \
    jint index = frame_stack_popi(frame); \
    jref arr = frame_stack_popr(frame); \
    if ((arr) == NULL) \
        jthread_throw_null_pointer_exception(frame->thread); \
    if (!arrobj_check_bounds(arr, index)) \
        jthread_throw_array_index_out_of_bounds_exception(frame->thread, index);

// ----- loads
void frame_iaload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushi(frame, arrobj_get(jint, arr, index));
}

void frame_faload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushf(frame, arrobj_get(jfloat, arr, index));
}

void frame_laload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushl(frame, arrobj_get(jlong, arr, index));
}

void frame_daload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushd(frame, arrobj_get(jdouble, arr, index));
}

void frame_aaload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushr(frame, arrobj_get(jref, arr, index));
}

void frame_baload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushi(frame, arrobj_get(jbyte, arr, index));
}

void frame_caload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushi(frame, arrobj_get(jchar, arr, index));
}

void frame_saload(struct frame *frame)
{
    GET_AND_CHECK_ARRAY
    frame_stack_pushi(frame, arrobj_get(jshort, arr, index));
}

// ----- stores
void frame_iastore(struct frame *frame)
{
    jint value = frame_stack_popi(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jint, arr, index, value);
}

void frame_fastore(struct frame *frame)
{
    jfloat value = frame_stack_popf(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jfloat, arr, index, value);
}

void frame_lastore(struct frame *frame)
{
    jlong value = frame_stack_popl(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jlong, arr, index, value);
}

void frame_dastore(struct frame *frame)
{
    jdouble value = frame_stack_popd(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jdouble, arr, index, value);
}

void frame_aastore(struct frame *frame)
{
    jref value = frame_stack_popr(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jref, arr, index, value);
}

void frame_bastore(struct frame *frame)
{
    jbyte value = (jbyte) frame_stack_popi(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jbyte, arr, index, value);
}

void frame_castore(struct frame *frame)
{
    jchar value = (jchar) frame_stack_popi(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jchar, arr, index, value);
}

void frame_sastore(struct frame *frame)
{
    jshort value = (jshort) frame_stack_popi(frame);
    GET_AND_CHECK_ARRAY
    arrobj_set(jshort, arr, index, value);
}

char* frame_to_string(const struct frame *f)
{
    assert(f != NULL);
    int len = strlen(f->m.method->clazz->class_name) + strlen(f->m.method->name) + strlen(f->m.method->descriptor) + 16;
    char *buf = malloc(sizeof(char) * len);
    strcpy(buf, IS_NATIVE(f->m.method->access_flags) ? "(native)" : "");
    strcat(buf, f->m.method->clazz->class_name);
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
