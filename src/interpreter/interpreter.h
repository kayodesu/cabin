#ifndef CABIN_INTERPRETER_H
#define CABIN_INTERPRETER_H

#include <initializer_list>
#include <cstdarg>
#include "../cabin.h"
#include "../metadata/method.h"
#include "../slot.h"

class Method;
class Array;

/*
 * 此方法用于虚拟机主动调用函数，
 * 函数调用指令（invokestatic, invokespecial, ...）中不能使用
 */

slot_t *execJavaFunc(Method *m, const slot_t *args = nullptr);

slot_t *execJavaFunc(Method *m, std::initializer_list<slot_t> args);

slot_t *execJavaFunc(Method *m, std::initializer_list<jref> args);

// Object[] args;
slot_t *execJavaFunc(Method *m, jref _this, Array *args);


#endif //CABIN_INTERPRETER_H
