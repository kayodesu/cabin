/*
 * Author: Jia Yang
 */

#ifndef JVM_OPERAND_STACK_H
#define JVM_OPERAND_STACK_H

#include <stdlib.h>
#include "../jtypes.h"

struct operand_stack;
struct slot;

struct operand_stack* os_create(u2 capacity);

void os_clear(struct operand_stack *os);

void os_destroy(struct operand_stack *os);

const struct slot* os_top(struct operand_stack *os);

struct slot* os_pops(struct operand_stack *os);

jint os_popi(struct operand_stack *os);
jfloat os_popf(struct operand_stack *os);
jlong os_popl(struct operand_stack *os);
jdouble os_popd(struct operand_stack *os);
jref os_popr(struct operand_stack *os);

void os_pushi(struct operand_stack *os, jint i);
void os_pushf(struct operand_stack *os, jfloat f);
void os_pushl(struct operand_stack *os, jlong l);
void os_pushd(struct operand_stack *os, jdouble d);
void os_pushr(struct operand_stack *os, jref r);

/*
 * 不判断 slot 的类型直接 push
 */
void os_push_slot_directly(struct operand_stack *os, const struct slot *s);

void os_pushs(struct operand_stack *os, const struct slot *s);

// 将一个值推入操作栈中
// os: 要推入值的 operand_stack 的地址
// v: 值
#define os_push(os, v) \
    _Generic((v), \
        jbyte: os_pushi, \
        jchar: os_pushi, \
        jshort: os_pushi, \
        jint: os_pushi, \
        jfloat: os_pushf, \
        jlong: os_pushl, \
        jdouble: os_pushd, \
        jref: os_pushr, \
        const struct slot *: os_pushs \
    )(os, v)

#endif //JVM_OPERAND_STACK_H
