/*
 * Author: Jia Yang
 */

#ifndef JVM_COMPARISONS_H
#define JVM_COMPARISONS_H

#include "../interpreter/stack_frame.h"

/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define CMP_RESULT(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

static void lcmp(struct stack_frame *frame)
{
    jlong v2 = os_popl(frame->operand_stack);
    jlong v1 = os_popl(frame->operand_stack);
    os_pushi(frame->operand_stack, CMP_RESULT(v1, v2, -1));
}

static void fcmpl(struct stack_frame *frame)
{
    jfloat v2 = os_popf(frame->operand_stack);
    jfloat v1 = os_popf(frame->operand_stack);
    os_pushi(frame->operand_stack, CMP_RESULT(v1, v2, -1));
}

static void fcmpg(struct stack_frame *frame)
{
    jfloat v2 = os_popf(frame->operand_stack);
    jfloat v1 = os_popf(frame->operand_stack);
    os_pushi(frame->operand_stack, CMP_RESULT(v1, v2, 1));
}

static void dcmpl(struct stack_frame *frame)
{
    jdouble v2 = os_popd(frame->operand_stack);
    jdouble v1 = os_popd(frame->operand_stack);
    os_pushi(frame->operand_stack, CMP_RESULT(v1, v2, -1));
}

static void dcmpg(struct stack_frame *frame)
{
    jdouble v2 = os_popd(frame->operand_stack);
    jdouble v1 = os_popd(frame->operand_stack);
    os_pushi(frame->operand_stack, CMP_RESULT(v1, v2, 1));
}

#undef CMP_RESULT

#define IF_ICMP(func_name, oper, cmp_with_0) \
    static void func_name(struct stack_frame *frame) \
    { \
        jint v2 = (cmp_with_0) ? 0 : os_popi(frame->operand_stack); \
        jint v1 = os_popi(frame->operand_stack); \
        int offset = bcr_reads2(frame->reader); \
        if (v1 oper v2) { \
            bcr_skip(frame->reader, offset - 3); /* 减3？减去本条指令自身的长度 todo */  \
        } \
    }

IF_ICMP(ifeq, ==, true)
IF_ICMP(ifne, !=, true)
IF_ICMP(iflt, <,  true)
IF_ICMP(ifge, >=, true)
IF_ICMP(ifgt, >,  true)
IF_ICMP(ifle, <=, true)

IF_ICMP(if_icmpeq, ==, false)
IF_ICMP(if_icmpne, !=, false)
IF_ICMP(if_icmplt, <,  false)
IF_ICMP(if_icmpge, >=, false)
IF_ICMP(if_icmpgt, >,  false)
IF_ICMP(if_icmple, <=, false)

static void if_acmpeq(struct stack_frame *frame)
{
    jref v2 = os_popr(frame->operand_stack);
    jref v1 = os_popr(frame->operand_stack);
    int offset = bcr_reads2(frame->reader);
    if (v1 == v2) {
        bcr_skip(frame->reader, offset - 3); /* todo 减3？减去本条指令自身的长度 */
    }
}

static void if_acmpne(struct stack_frame *frame)
{
    jref v2 = os_popr(frame->operand_stack);
    jref v1 = os_popr(frame->operand_stack);
    int offset = bcr_reads2(frame->reader);
    if (v1 != v2) {
        bcr_skip(frame->reader, offset - 3); /* todo 减3？减去本条指令自身的长度 */
    }
}

#endif //JVM_COMPARISONS_H
