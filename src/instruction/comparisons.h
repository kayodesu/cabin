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

static void ifeq(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) == 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void ifne(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) != 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void iflt(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) < 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void ifge(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) >= 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void ifgt(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) > 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void ifle(struct stack_frame *frame)
{
    jint offset = bcr_reads2(frame->reader);
    if (os_popi(frame->operand_stack) <= 0)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmpeq(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 == v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmpne(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 != v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmplt(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 < v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmpge(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 >= v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmpgt(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 > v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_icmple(struct stack_frame *frame)
{
    jint v2 = os_popi(frame->operand_stack);
    jint v1 = os_popi(frame->operand_stack);
    jint offset = bcr_reads2(frame->reader);
    if (v1 <= v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_acmpeq(struct stack_frame *frame)
{
    jref v2 = os_popr(frame->operand_stack);
    jref v1 = os_popr(frame->operand_stack);
    int offset = bcr_reads2(frame->reader);
    if (v1 == v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

static void if_acmpne(struct stack_frame *frame)
{
    jref v2 = os_popr(frame->operand_stack);
    jref v1 = os_popr(frame->operand_stack);
    int offset = bcr_reads2(frame->reader);
    if (v1 != v2)
        bcr_skip(frame->reader, offset - 3); // minus instruction length
}

#endif //JVM_COMPARISONS_H
