#include "../../rtda/thread/frame.h"
#include "../../rtda/heap/object.h"

/*
 * Author: Jia Yang
 */

void athrow(struct frame *frame)
{
    jref exception = frame_stack_popr(frame);
    if (exception == NULL) {
        jthread_throw_null_pointer_exception(frame->thread);
    }

    struct thread *curr_thread = frame->thread;

    // 遍历虚拟机栈找到可以处理此异常的方法
    while (!jthread_is_stack_empty(curr_thread)) {
        struct frame *top = jthread_top_frame(curr_thread);
        if (top->type == SF_TYPE_NORMAL) {
            int handler_pc = method_find_exception_handler(top->m.method, exception->clazz, top->reader.pc - 1); // instruction length todo 好像是错的
            if (handler_pc >= 0) {  // todo 可以等于0吗
                /*
                 * 找到可以处理的函数了
                 * 操作数栈清空
                 * 把异常对象引用推入栈顶
                 * 跳转到异常处理代码之前
                 */
                frame_stack_clear(top);
                frame_stack_pushr(top, exception);
                top->reader.pc = (size_t) handler_pc;
                frame_proc_exception(top);
                return;
            }
        }

        // top frame 无法处理异常，弹出
        jthread_pop_frame(curr_thread);
        if (top == frame) {
            // 当前执行的 frame 不能直接销毁，设置成执行完毕即可，由解释器销毁
            frame_exe_over(frame);
        } else {
            frame_destroy(top); // todo
        }
    }

    jthread_handle_uncaught_exception(curr_thread, exception);
}
