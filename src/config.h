/*
 * Author: kayo
 */

#ifndef JVM_CONFIG_H
#define JVM_CONFIG_H

#define VM_HEAP_SIZE  (64*1024*1024) // 64Mb

// every thread has a vm stack
#define VM_STACK_SIZE (64*1024)      // 64Kb

#define PRINT_EXECUTING_FRAME false
#define PRINT_EXECUTING_INSTRUCTION false

#endif //JVM_CONFIG_H
