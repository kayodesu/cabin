#include "../../../rtda/thread/frame.h"

/*
 * Author: Jia Yang
 */

void monitorenter(struct frame *frame)
{
    jref o = frame_stack_popr(frame); //os_popr(frame->operand_stack);

    // todo
//    thread := frame.Thread()
//    ref := frame.OperandStack().PopRef()
//    if ref == nil {
//        frame.RevertNextPC()
//        thread.ThrowNPE()
//    } else {
//        ref.Monitor().Enter(thread)
//    }
}