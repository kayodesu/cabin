/*
 * Author: Jia Yang
 */

#ifndef JVM_INSTRUCTION_H
#define JVM_INSTRUCTION_H

struct stack_frame;

/*
 * 指令
 */
struct instruction {
    int code;
    const char *name;
    void (* exec)(struct stack_frame *);
};

// 指令集
extern struct instruction instructions[];

#endif //JVM_INSTRUCTION_H
