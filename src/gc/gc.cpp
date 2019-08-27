/*
 * Author: kayo
 */

#include "gc.h"
#include "../kayo.h"
#include "../rtda/thread/Thread.h"
#include "../rtda/thread/Frame.h"

bool objectAccessible(jref obj)
{
    // 虚拟机栈(栈桢中的本地变量表)中的引用的对象
    for (Thread *thread : vmEnv.threads) {
        for (Frame *frame = thread->topFrame; frame != nullptr; frame = frame->prev) {
            if (frame->objectAccessible(obj))
                return true;
        }
    }

    // todo

    return false;
}