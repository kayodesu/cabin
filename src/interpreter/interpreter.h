/*
 * Author: Jia Yang
 */

#ifndef JVM_INTERPRETER_H
#define JVM_INTERPRETER_H

#include "../slot.h"

slot_t *exec_java_func(struct method *m, const slot_t *args, bool vm_invoke);
//void exec_java_func_true(struct method *method, slot_t *args);

//void* interpret(void *thread);

#endif //JVM_INTERPRETER_H
