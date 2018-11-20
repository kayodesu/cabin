/*
 * Author: Jia Yang
 */

#ifndef JVM_INTERPRETER_H
#define JVM_INTERPRETER_H

struct jthread;

void interpret(struct jthread *thread);

#endif //JVM_INTERPRETER_H
