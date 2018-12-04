/*
 * Author: Jia Yang
 */

#include "../interpreter/operand_stack.h"
#include "../interpreter/stack_frame.h"
#include "../rtda/ma/rtcp.h"
#include "../classfile/constant.h"
#include "../rtda/heap/strpool.h"
#include "../rtda/heap/jobject.h"

#ifndef JVM_CONSTANTS_H
#define JVM_CONSTANTS_H

#define tconst(func_name, value) \
static void func_name(struct stack_frame *frame) \
{ \
    os_push(frame->operand_stack, value); \
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

static void bipush(struct stack_frame *frame)
{
    jint i = bcr_reads1(frame->reader);
    os_pushi(frame->operand_stack, i);
}

static void sipush(struct stack_frame *frame)
{
    jint i = bcr_reads2(frame->reader);
    os_pushi(frame->operand_stack, i);
}

static void __ldc(struct stack_frame *frame, int index_bytes)
{
    int index;
    if (index_bytes == 1) {
        index = bcr_readu1(frame->reader);
    } else {
        index = bcr_readu2(frame->reader);
    }

    struct rtcp *rtcp = frame->method->jclass->rtcp;
    u1 type = rtcp_get_type(rtcp, index);

    struct operand_stack *os = frame->operand_stack;
    if (type == INTEGER_CONSTANT) {
        os_pushi(os, rtcp_get_int(rtcp, index));
    } else if (type == FLOAT_CONSTANT) {
        os_pushf(os, rtcp_get_float(rtcp, index));
    } else if (type == STRING_CONSTANT) {
//        printvm("ldc string\n");
        const char *str = rtcp_get_str(rtcp, index);
//        printvm("%s\n", str);
        struct jobject *so = get_str_from_pool(frame->method->jclass->loader, str);
#ifdef JVM_DEBUG
        JOBJECT_CHECK_STROBJ(so);
#endif
//        printvm("%p, %s\n", so, jstrobj_value(so));
        os_pushr(os, so);
    } else if (type == CLASS_CONSTANT) {
        const char *class_name = rtcp_get_class_name(rtcp, index);
//        printvm("ldc class, %s\n", class_name);  //////////////////////////////////////////////////
        struct jclass *c = classloader_load_class(frame->method->jclass->loader, class_name);
        os_pushr(os, c->clsobj);
    } else {
        jvm_abort("error. %d\n", type); // todo
    }
}

static void ldc(struct stack_frame *frame)
{
    __ldc(frame, 1);
}

static void ldc_w(struct stack_frame *frame)
{
    __ldc(frame, 2);
}

static void ldc2_w(struct stack_frame *frame)
{
    int index = bcr_readu2(frame->reader);
    struct rtcp *rtcp = frame->method->jclass->rtcp;
    u1 type = rtcp_get_type(rtcp, index);

    if (type == LONG_CONSTANT) {
        os_pushl(frame->operand_stack, rtcp_get_long(rtcp, index));
    } else if (type == DOUBLE_CONSTANT) {
        os_pushd(frame->operand_stack, rtcp_get_double(rtcp, index));
    } else {
        jvm_abort("error. %d\n", type);
    }
}

#endif //JVM_CONSTANTS_H