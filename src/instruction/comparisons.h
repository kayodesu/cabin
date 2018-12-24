/*
 * Author: Jia Yang
 */

#ifndef JVM_COMPARISONS_H
#define JVM_COMPARISONS_H

#include "../rtda/thread/frame.h"

/*
 * NAN 与正常的的浮点数无法比较，即 即不大于 也不小于 也不等于。
 * 两个 NAN 之间也无法比较，即 即不大于 也不小于 也不等于。
 */
#define CMP_RESULT(v1, v2, default_value) \
            (jint)((v1) > (v2) ? 1 : ((v1) == (v2) ? 0 : ((v1) < (v2) ? -1 : (default_value))))

static void lcmp(struct frame *frame)
{
    jlong v2 = frame_stack_popl(frame);
    jlong v1 = frame_stack_popl(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static void fcmpl(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static void fcmpg(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

static void dcmpl(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static void dcmpg(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

#undef CMP_RESULT

static void ifeq(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) == 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void ifne(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) != 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void iflt(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) < 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void ifge(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) >= 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void ifgt(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) > 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void ifle(struct frame *frame)
{
    jint offset = bcr_reads2(&frame->reader);
    if (frame_stack_popi(frame) <= 0)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmpeq(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 == v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmpne(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 != v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmplt(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 < v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmpge(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 >= v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmpgt(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 > v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_icmple(struct frame *frame)
{
    jint v2 = frame_stack_popi(frame);
    jint v1 = frame_stack_popi(frame);
    jint offset = bcr_reads2(&frame->reader);
    if (v1 <= v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_acmpeq(struct frame *frame)
{
    jref v2 = frame_stack_popr(frame);
    jref v1 = frame_stack_popr(frame);
    int offset = bcr_reads2(&frame->reader);
    if (v1 == v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

static void if_acmpne(struct frame *frame)
{
    jref v2 = frame_stack_popr(frame);
    jref v1 = frame_stack_popr(frame);
    int offset = bcr_reads2(&frame->reader);
    if (v1 != v2)
        bcr_skip(&frame->reader, offset - 3); // minus instruction length
}

#endif //JVM_COMPARISONS_H
