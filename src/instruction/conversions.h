/*
 * Author: Jia Yang
 */

#ifndef JVM_CONVERSIONS_H
#define JVM_CONVERSIONS_H

#include "../jtypes.h"
#include "../rtda/thread/frame.h"

#define x2y(x, y) \
static void __##x##2##y(struct frame *frame) \
{ \
    frame_stack_push##y(frame, x##2##y(frame_stack_pop##x(frame))); \
} \

x2y(i, l) x2y(i, f) x2y(i, d)
x2y(l, i) x2y(l, f) x2y(l, d)
x2y(f, i) x2y(f, l) x2y(f, d)
x2y(d, i) x2y(d, l)
//x2y(d, f)
static void __d2f(struct frame *frame)
{
    frame_stack_top(frame)->v.f = d2f(frame_stack_top(frame)->v.d);
}

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to a byte, then sign-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2b(struct frame *frame)
{
    frame_stack_pushi(frame, i2b(frame_stack_popi(frame)));
}

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to char, then zero-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2c(struct frame *frame)
{
    frame_stack_pushi(frame, i2c(frame_stack_popi(frame)));
}

/*
 * The value on the top of the operand stack must be of type int.
 * It is popped from the operand stack, truncated to a short, then sign-extended
 * to an int result. That result is pushed onto the operand stack.
 */
static void __i2s(struct frame *frame)
{
    frame_stack_pushi(frame, i2s(frame_stack_popi(frame)));
}


#endif //JVM_CONVERSIONS_H
