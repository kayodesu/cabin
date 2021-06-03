#ifndef CABIN_INTERPRETER_H
#define CABIN_INTERPRETER_H

#include "cabin.h"
#include "slot.h"

/*
 * 此方法用于虚拟机主动调用函数，
 * 函数调用指令（invokestatic, invokespecial, ...）中不能使用
 */

slot_t *exec_java(Method *, const slot_t *args);
#define exec_java_r(_method, _args) slot_get_ref(exec_java(_method, _args))

// Object[] args;
slot_t *exec_java0(Method *, jref _this, jarrRef args);

#endif // CABIN_INTERPRETER_H