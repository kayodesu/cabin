/*
 * Author: Jia Yang
 */

#ifndef JVM_STACKS_H
#define JVM_STACKS_H

#include "../interpreter/stack_frame.h"

// 弹出一个类型一的数据
static void pop(struct stack_frame *frame)
{
#ifdef JVM_DEBUG
    struct slot *s = os_pops(frame->operand_stack);
    if(!slot_is_category_one(s)) {
        jvm_abort(""); // todo
    }
#else
    os_pops(frame->operand_stack);
#endif
}

// 弹出两个类型一或一个类型二的数据
static void pop2(struct stack_frame *frame)
{
#ifdef JVM_DEBUG
    struct slot *top1 = os_pops(frame->operand_stack);
    struct slot *top2 = os_pops(frame->operand_stack);

    if( !(slots_are_category_two_and_ph(top2, top1) || slots_are_category_one(2, top1, top2)) ) {
        jvm_abort(""); // todo
    }
#else
    os_pops(frame->operand_stack);
    os_pops(frame->operand_stack);
#endif
}

// 复制栈顶数值（只支持分类一的数据）并将复制值压入栈顶。
static void dup(struct stack_frame *frame)
{
    const struct slot *s = os_top(frame->operand_stack);
#ifdef JVM_DEBUG
    if (!slot_is_category_one(s)) {
        jvm_abort(""); // todo
    }
#endif
    os_push_slot_directly(frame->operand_stack, s);
}

// 复制栈顶一个（分类二类型)或两个（分类一类型）数值并将复制值压入栈顶。
static void dup2(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    if( !(slots_are_category_two_and_ph(&top2, &top1) || slots_are_category_one(2, &top1, &top2)) ) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
}

/*
 * Duplicate the top value on the operand stack and insert the
 * duplicated value two values down in the operand stack.
 *
 * The dup_x1 instruction must not be used unless both value1 and
 * value2 are values of a category 1 computational type
 */
static void dup_x1(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    if (!slots_are_category_one(2, &top1, &top2)) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
}

// 复制操作数栈栈顶的值（类型一），
// 并插入到栈顶以下2个（栈顶类型一，栈顶下一个类型二）或3个（三个值都是类型一）值之后
static void dup_x2(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);
    const struct slot top3 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    bool f1 = slot_is_category_one(&top1) && slots_are_category_two_and_ph(&top3, &top2); // 栈顶类型一，栈顶下一个类型二
    bool f2 = slots_are_category_one(3, &top1, &top2, &top3);
    if ( !(f1 || f2) ) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top3);
    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
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
static void dup2_x1(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);
    const struct slot top3 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    bool f1 = slots_are_category_one(3, &top1, &top2, &top3); // Form 1
    bool f2 = slots_are_category_two_and_ph(&top2, &top1) && slot_is_category_one(&top3);  // Form 2
    if ( !(f1 || f2) ) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top3);
    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
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
static void dup2_x2(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);
    const struct slot top3 = *os_pops(frame->operand_stack);
    const struct slot top4 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    bool f1 = slots_are_category_one(4, &top1, &top2, &top3, &top4);                                      // Form 1
    bool f2 = slots_are_category_two_and_ph(&top2, &top1) && slots_are_category_one(2, &top3, &top4);     // Form 2
    bool f3 = slots_are_category_one(2, &top1, &top2) && slots_are_category_two_and_ph(&top4, &top3);     // Form 3
    bool f4 = slots_are_category_two_and_ph(&top4, &top3) && slots_are_category_two_and_ph(&top2, &top1); // Form 4
    if ( !(f1 || f2 || f3 || f4) ) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top4);
    os_push_slot_directly(frame->operand_stack, &top3);
    os_push_slot_directly(frame->operand_stack, &top2);
    os_push_slot_directly(frame->operand_stack, &top1);
}

/*
 * 交换操作数栈顶的两个值（都为类型一）
 *
 * The Java Virtual Machine does not provide an instruction
 * implementing a swap on operands of category 2 computational types.
 */
static void __swap(struct stack_frame *frame)
{
    const struct slot top1 = *os_pops(frame->operand_stack);
    const struct slot top2 = *os_pops(frame->operand_stack);

#ifdef JVM_DEBUG
    if (!slots_are_category_one(2, &top1, &top2)) {
        jvm_abort(""); // todo
    }
#endif

    os_push_slot_directly(frame->operand_stack, &top1);
    os_push_slot_directly(frame->operand_stack, &top2);
}

#endif //JVM_STACKS_H
