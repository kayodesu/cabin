/*
 * Author: Jia Yang
 */

#ifndef JVM_JVMDEF_H
#define JVM_JVMDEF_H

#include <cstdio>
#include <string.h>
#include <string>

#define JVM_DEBUG

// 打印虚拟机栈轨迹
#define VM_STACK_TRACE

// 打印指令轨迹
#define INSTRUCTION_TRACE


extern std::string bootstrapClasspath;
extern std::string extensionClasspath;
extern std::string userClasspath;

#define jprintf(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

// 出现异常，退出jvm
#define jvmAbort(...) do { jprintf("fatal error. "); printf(__VA_ARGS__); exit(-1); } while(false)

// string equals
static inline bool streq(const char *s1, const char *s2)
{
    return strcmp(s1, s2) == 0;
}

#endif //JVM_JVMDEF_H
