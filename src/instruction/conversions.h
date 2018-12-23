/*
 * Author: Jia Yang
 */

#ifndef JVM_CONVERSIONS_H
#define JVM_CONVERSIONS_H

#include "../jtypes.h"
#include "../interpreter/stack_frame.h"

#define x2y(x, y) \
static void __##x##2##y(struct stack_frame *frame) \
{ \
    os_push##y(frame->operand_stack, x##2##y(os_pop##x(frame->operand_stack))); \
} \

x2y(i, l) x2y(i, f) x2y(i, d)
x2y(l, i) x2y(l, f) x2y(l, d)
x2y(f, i) x2y(f, l) x2y(f, d)
x2y(d, i) x2y(d, l) x2y(d, f)

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to a byte, then sign-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2b(struct stack_frame *frame)
{
    os_pushi(frame->operand_stack, i2b(os_popi(frame->operand_stack)));
}

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to char, then zero-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2c(struct stack_frame *frame)
{
    os_pushi(frame->operand_stack, i2c(os_popi(frame->operand_stack)));
}

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to a short, then sign-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2s(struct stack_frame *frame)
{
    os_pushi(frame->operand_stack, i2s(os_popi(frame->operand_stack)));
}


#endif //JVM_CONVERSIONS_H
