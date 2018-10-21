/*
 * Author: Jia Yang
 */

#ifndef JVM_STACK_FRAME_H
#define JVM_STACK_FRAME_H

#include <stdbool.h>
#include "operand_stack.h"
#include "../rtda/ma/jmethod.h"
#include "../rtda/thread/jthread.h"
#include "../util/bytecode_reader.h"

struct stack_frame {
    struct slot *local_vars; // 局部变量表
    int max_locals;  // todo

    bool interrupted_status;
    bool exe_status; // todo 是否执行完毕
    bool proc_exception_status; // 处理异常

    struct operand_stack *operand_stack; // 操作栈

    struct bytecode_reader *reader;

    struct jmethod *method;
    struct jthread *thread;
};

struct stack_frame* sf_create(struct jthread *thread, struct jmethod *method);

void sf_set_local_var(struct stack_frame *frame, int index, const struct slot *value);
void sf_invoke_method(struct stack_frame *frame, struct jmethod *method, const struct slot *args);

void sf_destroy(struct stack_frame *frame);


static inline void sf_proc_exception(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->proc_exception_status = true;
}

/*
 * todo 正在处理异常？ 用 sf_is_procing_exception 会不会好一点
 */
static inline bool sf_is_proc_exception(struct stack_frame *frame)
{
    assert(frame != NULL);
    return frame->proc_exception_status;
}

static inline void sf_exe_over(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->exe_status = true;
}

static inline bool sf_is_exe_over(struct stack_frame *frame)
{
    assert(frame != NULL);
    return frame->exe_status;
}

/*
 * Interrupts the function of this StackFrame.
 */
static inline void sf_interrupt(struct stack_frame *frame)
{
    assert(frame != NULL);
    frame->interrupted_status = true;
}

/*
 * Tests whether the current StackFrame has been interrupted.
 * The interrupted status of the StackFrame is cleared by this method.
 *
 * return: true if the current StackFrame has been interrupted;
 *         false otherwise.
 */
static inline bool sf_interrupted(struct stack_frame *frame)
{
    assert(frame != NULL);

    bool tmp = frame->interrupted_status;
    frame->interrupted_status = false;
    return tmp;
}

char* sf_to_string(const struct stack_frame *frame);


#endif //JVM_STACK_FRAME_H
