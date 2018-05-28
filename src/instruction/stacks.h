/*
 * Author: Jia Yang
 */

#ifndef JVM_STACKS_H
#define JVM_STACKS_H

#include "../interpreter/StackFrame.h"

// 弹出一个类型一的数据
static void pop(StackFrame *frame) {
    frame->operandStack.popSlot();
}

// 弹出两个类型一或一个类型二的数据
static void pop2(StackFrame *frame) {
    frame->operandStack.popSlot();
    frame->operandStack.popSlot();
}

// 复制栈顶数值（只支持分类一的数据）并将复制值压入栈顶。
static void dup(StackFrame *frame) {
    Slot *top = frame->operandStack.peek();
    frame->operandStack.directPush(top);
}

// 复制栈顶一个（分类二类型)或两个（分类一类型）数值并将复制值压入栈顶。
static void dup2(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top = os.popSlot();
    Slot *underTop = os.popSlot();

    os.directPush(underTop);
    os.directPush(top);
    os.directPush(underTop);
    os.directPush(top);
}

// 复制操作数栈栈顶的值，并插入到栈顶以下2个值之后。
static void dupX1(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top = os.popSlot();
    Slot *underTop = os.popSlot();

    os.directPush(top);
    os.directPush(underTop);
    os.directPush(top);
}

// 复制操作数栈栈顶的值，并插入到栈顶以下2个或3个值之后
static void dupX2(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top = os.popSlot();
    Slot *underTop = os.popSlot();
    Slot *thirdTop = os.popSlot();

    os.directPush(top);
    os.directPush(thirdTop);
    os.directPush(underTop);
    os.directPush(top);
}

// dup_x1指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个或3个值之后
static void dup2X1(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top = os.popSlot();
    Slot *underTop = os.popSlot();
    Slot *thirdTop = os.popSlot();

    os.directPush(underTop);
    os.directPush(top);
    os.directPush(thirdTop);
    os.directPush(underTop);
    os.directPush(top);
}

// dup_x2指令的双倍版本。复制操作数栈栈顶1个或2个值，并插入到栈顶以下2个、3个或者4个值之后
static void dup2X2(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top0 = os.popSlot();
    Slot *top1 = os.popSlot();
    Slot *top2 = os.popSlot();
    Slot *top3 = os.popSlot();

    os.directPush(top1);
    os.directPush(top0);
    os.directPush(top3);
    os.directPush(top2);
    os.directPush(top1);
    os.directPush(top0);
}

// 交换操作数栈顶的两个值
static void __swap(StackFrame *frame) {
    OperandStack &os = frame->operandStack;
    Slot *top = os.popSlot();
    Slot *underTop = os.popSlot();

    os.directPush(top);
    os.directPush(underTop);
}

#endif