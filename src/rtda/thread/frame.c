/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include <assert.h>
#include "frame.h"
#include "../ma/access.h"
#include "../heap/object.h"
#include "../heap/arrobj.h"
#include "thread.h"


#define GET_AND_CHECK_ARRAY \
    jint index = frame_stack_popi(frame); \
    jref arr = frame_stack_popr(frame); \
    if ((arr) == NULL) \
        thread_throw_null_pointer_exception(); \
    if (!arrobj_check_bounds(arr, index)) \
        thread_throw_array_index_out_of_bounds_exception(index);

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
    int len = strlen(f->method->clazz->class_name) + strlen(f->method->name) + strlen(f->method->descriptor) + 16;
    char *buf = malloc(sizeof(char) * len);
    strcpy(buf, IS_NATIVE(f->method->access_flags) ? "(native)" : "");
    strcat(buf, f->method->clazz->class_name);
    strcat(buf, "~");
    strcat(buf, f->method->name);
    strcat(buf, "~");
    strcat(buf, f->method->descriptor);
    return buf; // todo
}

void frame_destroy(struct frame *f)
{
    if (f == NULL)
        return;

    // todo
//    free(frame);
}
