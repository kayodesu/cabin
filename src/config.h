/*
 * Author: kayo
 */

#ifndef JVM_CONFIG_H
#define JVM_CONFIG_H

// jvm 最大支持的classfile版本
#define JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION 57
#define JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION 65535

#define VM_HEAP_SIZE (64*1024*1024) // 64Mb

// every thread has a vm stack
#define VM_STACK_SIZE (64*1024)     // 64Kb

#endif //JVM_CONFIG_H
