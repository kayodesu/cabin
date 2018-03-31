/*
 * Author: Jia Yang
 */

#include <tuple>
#include <cassert>
#include "../rtda/thread/Jthread.h"
#include "../rtda/heap/methodarea/Jmethod.h"

using namespace std;

extern tuple<int, const char *, void (*)(StackFrame *)> instructions[];

void interpret(Jthread *thread) {
    while (!thread->isStackEmpty()) {
        StackFrame *frame = thread->topFrame();

        BytecodeReader *reader = frame->reader;

#ifdef VM_STACK_TRACE
        jprintf("executing frame: %s, pc = %lu\n", frame->toString().c_str(), reader->pc);
#endif

        while (reader->hasMore()) {
            frame->thread->pc = reader->pc;
            u1 opcode = reader->readu1();

#ifdef INSTRUCTION_TRACE
            jprintf("%d(0x%x), %s, pc = %lu\n", opcode, opcode, get<1>(instructions[opcode]), reader->pc);
#endif

            assert(get<0>(instructions[opcode]) == opcode);
            assert(get<2>(instructions[opcode]) != nullptr);

            get<2>(instructions[opcode])(frame);

            if (frame->isExeOver()) {
                delete frame;
                break;
            }

            if (frame->interrupted()) {
                break; // 当前函数执行被中断，跳出循环，终止当前 Frame 的执行。
            }

            /*
             * 如果当前栈顶帧正在处理异常，
             * 则需重置frame和reader，因为在异常处理中可能有一些帧被弹出
             */
            if (thread->topFrame()->isProcException()) {
                frame = thread->topFrame();
                reader = frame->reader;
            }
        }
    }

    jprintf("interpret exit.\n");
}
