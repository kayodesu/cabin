/*
 * Author: Jia Yang
 */

#ifndef JVM_EXTENDED_H
#define JVM_EXTENDED_H

#include "../interpreter/stack_frame.h"

static void multianewarray(struct stack_frame *frame)
{
#if 0
    // todo
    int index = frame->reader->readu2();
    const string &className = frame->method->jclass->rtcp->getClassName(index);
    JClass *arrClass = resolveClass(frame->method->jclass, className);

    jint arrDim = frame->reader->readu1();
    jint arrLens[arrDim];
    for (jint i = arrDim - 1; i >= 0; i--) {
        arrLens[i] = frame->operandStack.popInt();
        if (arrLens[i] < 0) {
            // todo  java.lang.NegativeArraySizeException
            jvmAbort("error. java.lang.NegativeArraySizeException \n");
            return;
        }
    }

    frame->operandStack.push(JArrayObj::newJArrayObj(arrClass, arrDim, arrLens));
#endif
}

static void ifnull(struct stack_frame *frame)
{
    int offset = bcr_readu2(frame->reader);//frame->reader->readu2();

    if (os_popr(frame->operand_stack) == NULL) {
        bcr_skip(frame->reader, offset - 3); // why减3？减去本条指令自身的长度
    }
}

static void ifnonnull(struct stack_frame *frame)
{
    int offset = bcr_readu2(frame->reader);//frame->reader->readu2();

    if (os_popr(frame->operand_stack) != NULL) {
        bcr_skip(frame->reader, offset - 3); // why减3？减去本条指令自身的长度
    }
}

extern bool wide_extending;

static void wide(struct stack_frame *frame)
{
    wide_extending = true;
}

#endif //JVM_EXTENDED_H
