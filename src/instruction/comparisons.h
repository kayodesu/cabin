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

static inline void lcmp(struct frame *frame)
{
    jlong v2 = frame_stack_popl(frame);
    jlong v1 = frame_stack_popl(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void fcmpl(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void fcmpg(struct frame *frame)
{
    jfloat v2 = frame_stack_popf(frame);
    jfloat v1 = frame_stack_popf(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

static inline void dcmpl(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, -1));
}

static inline void dcmpg(struct frame *frame)
{
    jdouble v2 = frame_stack_popd(frame);
    jdouble v1 = frame_stack_popd(frame);
    frame_stack_pushi(frame, CMP_RESULT(v1, v2, 1));
}

#undef CMP_RESULT

#define IF_COND(cond) \
    do { \
        jint v = frame_stack_popi(frame); \
        jint offset = frame_reads2(frame); \
        if (v cond 0) \
            frame_skip(frame, offset - 3);  /* minus instruction length */ \
    } while(false)

#define IF_ICMP_COND(cond) \
    do { \
        jint v2 = frame_stack_popi(frame); \
        jint v1 = frame_stack_popi(frame); \
        jint offset = frame_reads2(frame); \
        if (v1 cond v2) \
            frame_skip(frame, offset - 3); /* minus instruction length */ \
    } while(false)

#define IF_ACMP_COND(cond) \
    do { \
        jref v2 = frame_stack_popr(frame); \
        jref v1 = frame_stack_popr(frame); \
        jint offset = frame_reads2(frame); \
        if (v1 cond v2) \
            frame_skip(frame, offset - 3);  /* minus instruction length */ \
    } while (false)

#endif //JVM_COMPARISONS_H
