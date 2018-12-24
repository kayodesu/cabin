/*
 * Author: Jia Yang
 */

#include "../rtda/thread/frame.h"
#include "../rtda/ma/rtcp.h"
#include "../classfile/constant.h"
#include "../rtda/heap/strpool.h"
#include "../rtda/heap/jobject.h"

#ifndef JVM_CONSTANTS_H
#define JVM_CONSTANTS_H

#define tconst(func_name, value) \
static void func_name(struct frame *frame) \
{ \
    frame_stack_push(frame, value); \
}

tconst(aconst_null, (jref) NULL)

tconst(iconst_m1, (jint) -1)
tconst(iconst_0, (jint) 0)
tconst(iconst_1, (jint) 1)
tconst(iconst_2, (jint) 2)
tconst(iconst_3, (jint) 3)
tconst(iconst_4, (jint) 4)
tconst(iconst_5, (jint) 5)

tconst(lconst_0, (jlong) 0)
tconst(lconst_1, (jlong) 1)

tconst(fconst_0, (jfloat) 0)
tconst(fconst_1, (jfloat) 1)
tconst(fconst_2, (jfloat) 2)

tconst(dconst_0, (jdouble) 0)
tconst(dconst_1, (jdouble) 1)

static void bipush(struct frame *frame)
{
    jint i = bcr_reads1(&frame->reader);
    frame_stack_pushi(frame, i);
}

static void sipush(struct frame *frame)
{
    jint i = bcr_reads2(&frame->reader);
    frame_stack_pushi(frame, i);
}

static void __ldc(struct frame *frame, int index_bytes)
{
    int index;
    if (index_bytes == 1) {
        index = bcr_readu1(&frame->reader);
    } else {
        index = bcr_readu2(&frame->reader);
    }

    struct rtcp *rtcp = frame->m.method->jclass->rtcp;
    u1 type = rtcp_get_type(rtcp, index);

    if (type == INTEGER_CONSTANT) {
        frame_stack_pushi(frame, rtcp_get_int(rtcp, index));
    } else if (type == FLOAT_CONSTANT) {
        frame_stack_pushf(frame, rtcp_get_float(rtcp, index));
    } else if (type == STRING_CONSTANT) {
        const char *str = rtcp_get_str(rtcp, index);
        struct jobject *so = get_str_from_pool(frame->m.method->jclass->loader, str);
        frame_stack_pushr(frame, so);
    } else if (type == CLASS_CONSTANT) {
        const char *class_name = rtcp_get_class_name(rtcp, index);
        struct jclass *c = classloader_load_class(frame->m.method->jclass->loader, class_name);
        frame_stack_pushr(frame, c->clsobj);
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
}

static void ldc(struct frame *frame)
{
    __ldc(frame, 1);
}

static void ldc_w(struct frame *frame)
{
    __ldc(frame, 2);
}

static void ldc2_w(struct frame *frame)
{
    int index = bcr_readu2(&frame->reader);
    struct rtcp *rtcp = frame->m.method->jclass->rtcp;
    u1 type = rtcp_get_type(rtcp, index);

    if (type == LONG_CONSTANT) {
        frame_stack_pushl(frame, rtcp_get_long(rtcp, index));
    } else if (type == DOUBLE_CONSTANT) {
        frame_stack_pushd(frame, rtcp_get_double(rtcp, index));
    } else {
        jvm_abort("error. %d\n", type);
    }
}

#endif //JVM_CONSTANTS_H