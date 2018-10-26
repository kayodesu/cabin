/*
 * Author: Jia Yang
 */

#include "interpreter.h"
#include "../instruction/instruction.h"

extern struct instruction instructions[];

void interpret(struct jthread *thread)
{
    while (!jthread_is_stack_empty(thread)) {
        struct stack_frame *frame = jthread_top_frame(thread);
        struct bytecode_reader *reader = frame->reader;

        if (verbose)
            printvm("executing frame: %s, pc = %lu\n", sf_to_string(frame), reader->pc);

        while (bcr_has_more(reader)) {
            frame->thread->pc = reader->pc;
            u1 opcode = bcr_readu1(reader);

            if (verbose)
                printvm("%d(0x%x), %s, pc = %lu\n", opcode, opcode, instructions[opcode].name, reader->pc);

            assert(instructions[opcode].code == opcode);
            assert(instructions[opcode].exec != NULL);

            instructions[opcode].exec(frame);

            if (sf_is_exe_over(frame)) {
                sf_destroy(frame);
                break;
            }

            if (sf_interrupted(frame)) {
                break; // 当前函数执行被中断。跳出循环，终止当前 Frame 的执行。
            }

            /*
             * 如果当前栈顶帧正在处理异常，
             * 则需重置frame和reader，因为在异常处理中可能有一些帧被弹出
             */
            if (sf_is_proc_exception(jthread_top_frame(thread))) {
                frame = jthread_top_frame(thread);
                reader = frame->reader;
            }
        }
    }

    printvm("interpret exit.\n");
}
