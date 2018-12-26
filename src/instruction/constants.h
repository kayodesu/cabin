/*
 * Author: Jia Yang
 */

#include "../rtda/thread/frame.h"
#include "../rtda/ma/rtcp.h"
#include "../classfile/constant.h"
#include "../rtda/heap/strpool.h"
#include "../rtda/heap/object.h"

#ifndef JVM_CONSTANTS_H
#define JVM_CONSTANTS_H

static void __ldc(struct frame *frame, int index)
{
    struct rtcp *rtcp = frame->m.method->jclass->rtcp;
    u1 type = rtcp_get_type(rtcp, index);

    if (type == INTEGER_CONSTANT) {
        frame_stack_pushi(frame, rtcp_get_int(rtcp, index));
    } else if (type == FLOAT_CONSTANT) {
        frame_stack_pushf(frame, rtcp_get_float(rtcp, index));
    } else if (type == STRING_CONSTANT) {
        const char *str = rtcp_get_str(rtcp, index);
        struct object *so = get_str_from_pool(frame->m.method->jclass->loader, str);
        frame_stack_pushr(frame, so);
    } else if (type == CLASS_CONSTANT) {
        const char *class_name = rtcp_get_class_name(rtcp, index);
        struct class *c = classloader_load_class(frame->m.method->jclass->loader, class_name);
        frame_stack_pushr(frame, c->clsobj);
    } else {
        VM_UNKNOWN_ERROR("unknown type: %d", type);
    }
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