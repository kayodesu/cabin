/*
 * Author: Jia Yang
 */

#ifndef JVM_OPERAND_STACK_H
#define JVM_OPERAND_STACK_H

#include <stdlib.h>
#include "../slot.h"
#include "../jtypes.h"

/*
 * 操作栈。
 * 栈容量在编译期已确定，所以此栈不会存在溢出的问题。
 */
struct operand_stack {
    u2 capacity; // 总容量
    u2 size;     // 当前大小
    struct slot *slots;
};

struct operand_stack* os_create(u2 capacity);

static inline void os_clear(struct operand_stack *os)
{
    assert(os != NULL);
    os->size = 0;
}

void os_destroy(struct operand_stack *os);

struct slot* os_top(struct operand_stack *os);

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


#if 0
public:
    explicit OperandStack(u2 capacity): capacity(capacity), slots(new Slot[capacity]) { }

    ~OperandStack() {
        delete[] slots;
    }

    void push(jint v) {
        slots[size++].set(v);
    }

    void push(jfloat v) {
        slots[size++].set(v);
    }

    void push(jref v) {
        slots[size++].set(v);
    }

    // long consumes two slots
    void push(jlong v) {
        slots[size++].set(v);
        slots[size++].ph();
    }

    // double consumes two slots
    void push(jdouble v) {
        slots[size++].set(v);
        slots[size++].ph();
    }

    void push(const Slot& s) {
        slots[size] = s;
        size += (s.isCategoryTwo() ? 2 : 1);
    }

//    void directPush(const Slot& s) {
//        slots[size++] = s;
//    }


    jbyte popByte() {
        return jtypes::i2b(slots[--size].getInt());
    }

    jchar popChar() {
        return jtypes::i2c(slots[--size].getInt());
    }

    jshort popShort() {
        return jtypes::i2s(slots[--size].getInt());
    }

    jint popInt() {
        return slots[--size].getInt();
    }

    jfloat popFloat() {
        return slots[--size].getFloat();
    }

    jref popRef() {
        return slots[--size].getRef();
    }

    jlong popLong() {
        size -= 2;
        return slots[size].getLong();
    }

    jdouble popDouble() {
        size -= 2;
        return slots[size].getDouble();
    }

    Slot popSlot() {
        return slots[--size];
    }

    const Slot& top() const {
        return slots[size - 1];
    }

    void clear() {
        size = 0;
    }

#endif

#endif //JVM_OPERAND_STACK_H
