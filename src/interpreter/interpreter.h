/*
 * Author: kayo
 */

#ifndef JVM_INTERPRETER_H
#define JVM_INTERPRETER_H

#include <initializer_list>
#include <cstdarg>
#include "../vmdef.h"
#include "../kayo.h"
#include "../native/jni.h"
#include "../objects/signature.h"
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

static inline slot_t *execJavaFunc(Method *m, jref _this, const jvalue *args)
{
    // todo
    jvm_abort("not implement.");
}

static inline slot_t *execJavaFunc(Method *m, jref _this, va_list &args)
{
    jvalue values[METHOD_PARAMETERS_MAX_COUNT];
    parse_method_args_va_list(m->signature, args, values);
    return execJavaFunc(m, _this, values);
}

static inline slot_t *execJavaFunc(Method *m, jref _this, jclsref c, const jvalue *args)
{
    // todo
    jvm_abort("not implement.");
}

static inline slot_t *execJavaFunc(Method *m, jref _this, jclsref c, va_list &args)
{
    jvalue values[METHOD_PARAMETERS_MAX_COUNT];
    parse_method_args_va_list(m->signature, args, values);
    return execJavaFunc(m, _this, c, values);
}

// Object[] args;
slot_t *execConstructor(Method *constructor, jref _this, Array *args);


static inline slot_t *execConstructor(Method *constructor, jref _this, const jvalue *args)
{
    // todo
    jvm_abort("not implement.");
}

static inline slot_t *execConstructor(Method *constructor, jref _this, va_list &args)
{
    jvalue values[METHOD_PARAMETERS_MAX_COUNT];
    parse_method_args_va_list(constructor->signature, args, values);
    return execConstructor(constructor, _this, values);
}

#endif //JVM_INTERPRETER_H
