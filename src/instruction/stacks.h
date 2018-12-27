/*
 * Author: Jia Yang
 */

#ifndef JVM_STACKS_H
#define JVM_STACKS_H

#include "../rtda/thread/frame.h"

// 复制栈顶数值（只支持分类一的数据）并将复制值压入栈顶。
static inline void dup(struct frame *frame)
{
    assert(frame != NULL);
    frame->stack[frame->stack_top + 1] = frame->stack[frame->stack_top];
    frame->stack_top++;
}

/*
 * Duplicate the top value on the operand stack and insert the
 * duplicated value two values down in the operand stack.
 *
 * The dup_x1 instruction must not be used unless both value1 and
 * value2 are values of a category 1 computational type
 */
static inline void dup_x1(struct frame *frame)
{
    assert(frame != NULL);
    frame->stack[frame->stack_top + 1] = frame->stack[frame->stack_top];
    frame->stack[frame->stack_top] = frame->stack[frame->stack_top - 1];
    frame->stack[frame->stack_top - 1] = frame->stack[frame->stack_top + 1];
    frame->stack_top++;
}

// 复制操作数栈栈顶的值（类型一），
// 并插入到栈顶以下2个（栈顶类型一，栈顶下一个类型二）或3个（三个值都是类型一）值之后
static inline void dup_x2(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, -1), FSFT(frame, -2), 3 * sizeof(*(frame->stack)));
    frame->stack[frame->stack_top - 2] = frame->stack[frame->stack_top + 1];
    frame->stack_top++;

}

// 复制栈顶一个（分类二类型)或两个（分类一类型）数值并将复制值压入栈顶。
static inline void dup2(struct frame *frame)
{
    assert(frame != NULL);
    memcpy(FSFT(frame, 1), FSFT(frame, -1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
    dup_x1指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个或3个值之后。

    Form 1:
    ..., value3, value2, value1 →
    ..., value2, value1, value3, value2, value1
    where value1, value2, and value3 are all values of a category 1 computational type.

    Form 2:
    ..., value2, value1 →
    ..., value1, value2, value1
    where value1 is a value of a category 2 computational type
    and value2 is a value of a category 1 computational type.
 */
static inline void dup2_x1(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, 0), FSFT(frame, -2), 3 * sizeof(*(frame->stack)));
    memmove(FSFT(frame, -2), FSFT(frame, 1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
    dup_x2指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个、3个或者4个值之后

    Form 1:
    ..., value4, value3, value2, value1 →
    ..., value2, value1, value4, value3, value2, value1
    where value1, value2, value3, and value4 are all values of a category 1 computational type.

    Form 2:
    ..., value3, value2, value1 →
    ..., value1, value3, value2, value1
    where value1 is a value of a category 2 computational type and
    value2 and value3 are both values of a category 1 computational type.

    Form 3:
    ..., value3, value2, value1 →
    ..., value2, value1, value3, value2, value1
    where value1 and value2 are both values of a category 1
    computational type and value3 is a value of a category 2 computational type.

    Form 4:
    ..., value2, value1 →
    ..., value1, value2, value1
    where value1 and value2 are both values of a category 2 computational type.
 */
static inline void dup2_x2(struct frame *frame)
{
    assert(frame != NULL);
    memmove(FSFT(frame, -1), FSFT(frame, -3), 3 * sizeof(*(frame->stack)));
    memmove(FSFT(frame, -3), FSFT(frame, 1), 2 * sizeof(*(frame->stack)));
    frame->stack_top += 2;
}

/*
 * 交换操作数栈顶的两个值（都为类型一）
 *
 * The Java Virtual Machine does not provide an instruction
 * implementing a swap on operands of category 2 computational types.
 */
static inline void __swap(struct frame *frame)
{
    assert(frame != NULL);
    const struct slot top1 = *frame_stack_pop_slot(frame);
    const struct slot top2 = *frame_stack_pop_slot(frame);

    frame_stack_push_slot_directly(frame, &top1);
    frame_stack_push_slot_directly(frame, &top2);
}

#endif //JVM_STACKS_H
