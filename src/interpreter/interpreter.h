/*
 * Author: kayo
 */

#ifndef JVM_INTERPRETER_H
#define JVM_INTERPRETER_H

#include <initializer_list>
#include <cstdarg>
#include "../jvmstd.h"
#include "../objects/method.h"
#include "../slot.h"

class Method;
class Array;

/*
 * 此方法用于虚拟机主动调用函数，
 * 函数调用指令（invokestatic, invokespecial, ...）中不能使用
 */

slot_t *execJavaFunc(Method *m, const slot_t *args = nullptr);

slot_t *execJavaFunc(Method *m, std::initializer_list<slot_t> args);

static inline slot_t *execJavaFunc(Method *m, jref o)
{
    return execJavaFunc(m, { slot::rslot(o) });
}

static inline slot_t *execJavaFunc(Method *m, jref o1, jref o2)
{
    return execJavaFunc(m, { slot::rslot(o1), slot::rslot(o2) });
}

static inline slot_t *execJavaFunc(Method *m, jref o1, jref o2, jref o3)
{
    return execJavaFunc(m, { slot::rslot(o1), slot::rslot(o2), slot::rslot(o3) });
}

static inline slot_t *execJavaFunc(Method *m, jref o1, jref o2, jref o3, jref o4)
{
    return execJavaFunc(m, { slot::rslot(o1), slot::rslot(o2), slot::rslot(o3), slot::rslot(o4) });
}

// Object[] args;
slot_t *execConstructor(Method *constructor, jref _this, Array *args);


#endif //JVM_INTERPRETER_H
