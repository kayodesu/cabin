/*
 * Author: Jia Yang
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "jtypes.h"

#ifndef PATH_MAX
#define PATH_MAX 260 // todo
#endif

// todo
//#define JVM_STRING_LEN_MAX (0xFFFF - 1)
//#define JVM_PKT_NAME_LEN_MAX JVM_STRING_LEN_MAX
//#define JVM_CLASS_NAME_LEN_MAX JVM_STRING_LEN_MAX

// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
extern int jre_lib_jars_count;
extern char jre_lib_jars[][PATH_MAX];

// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
extern int jre_ext_jars_count;
extern char jre_ext_jars[][PATH_MAX];

/*
 * 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
 *
 * 用户类路径的默认值是当前目录。可以设置CLASSPATH环境变量来修改用户类路径。
 * 可以通过 -cp 选项修改，
 * -cp 选项的优先级更高，可以覆盖CLASSPATH环境变量设置。
 * -cp 选项既可以指定目录，也可以指定JAR文件。
 */
extern int user_dirs_count;
extern int user_jars_count;
extern char user_dirs[][PATH_MAX];
extern char user_jars[][PATH_MAX];

// 初始堆大小
//extern size_t g_initial_heap_size;

extern struct heap_mgr g_heap_mgr;

// todo 说明
extern struct classloader *g_bootstrap_loader;

#define OBJ "java/lang/Object"
#define CLS "java/lang/Class"
#define STR "java/lang/String"

#define LOBJ "Ljava/lang/Object;"
#define LCLS "Ljava/lang/Class;"
#define LSTR "Ljava/lang/String;"
#define LCLD "Ljava/lang/ClassLoader;"

static inline void* vm_malloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
        vm_out_of_memory_error("malloc failed");
    return p;
}

static inline void* vm_calloc(size_t n, size_t size)
{
    void *p = calloc(n, size);
    if (p == NULL)
        vm_out_of_memory_error("malloc failed");
    return p;
}

/*
 * jvms规定函数最多有255个参数，this也算，long和double占两个长度
 */
#define METHOD_PARAMETERS_MAX_COUNT 255

/*
 * struct 中标记为 NO_ACCESS 的成员拒绝直接访问，
 * 应通过对应的 get function 获取（比如此成员可能因为效率原因采用了延迟加载）。
 */
#define NO_ACCESS

// The system Thread group.
extern struct object *system_thread_group;

// name of main thread
#define MAIN_THREAD_NAME "main"

#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

/*
 * 是否开启调试模式(true or false)
 */
#define JVM_DEBUG 0

/*
 * print level.
 * 0: only print errors.
 * 1: print warnings.
 * 2: print 函数调用
 * 3: print every instructions
 */
#define PRINT_LEVEL 0

#define print0(...) do { printvm("fatal error. "); printf(__VA_ARGS__); } while(false)

#if (PRINT_LEVEL >= 1)
#define print1 printvm
#else
#define print1(...)
#endif

#if (PRINT_LEVEL >= 2)
#define print2 printvm
#else
#define print2(...)
#endif

#if (PRINT_LEVEL >= 3)
#define print3 printvm
#else
#define print3(...)
#endif


// 出现异常，退出jvm
#define jvm_abort(...) do { print0(__VA_ARGS__); exit(-1); } while(false)

/*
 * Virtual Machine Errors
 * A Java Virtual Machine implementation throws an object that is an instance of
 * a subclass of the class VirtualMachineError when an internal error or resource
 * limitation prevents it from implementing the semantics described in this chapter.
 * This specification cannot predict where internal errors or resource limitations may
 * be encountered and does not mandate precisely when they can be reported. Thus,
 * any of the VirtualMachineError subclasses defined below may be thrown at any
 * time during the operation of the Java Virtual Machine
 */
#define ERR_MSG_MAX_LEN 1023

/*
 * java.lang.InternalError
 *
 * An internal error has occurred in the Java Virtual Machine implementation
 * because of a fault in the software implementing the virtual machine,
 * a fault in the underlying host system software, or a fault in the hardware.
 * This error is delivered asynchronously when it is detected and may occur at any point in a program.
 */
_Noreturn void vm_internal_error(const char *msg);

/*
 * java.lang.OutOfMemoryError
 *
 * The Java Virtual Machine implementation has run out of either virtual or physical memory,
 * and the automatic storage mgr was unable to reclaim enough memory to satisfy an object creation request.
 */
_Noreturn void vm_out_of_memory_error(const char *msg);

/*
 * java.lang.StackOverflowError
 *
 * The Java Virtual Machine implementation has run out of stack space for a thread,
 * typically because the thread is doing an unbounded number of recursive invocations
 * as a result of a fault in the executing program.
 */
_Noreturn void vm_stack_overflow_error();

/*
 * java.lang.UnknownError
 *
 * An exception or error has occurred, but the Java Virtual Machine
 * implementation is unable to report the actual exception or error.
 */
_Noreturn void vm_unknown_error(const char *msg);

#define VM_UNKNOWN_ERROR(...) \
        do { \
            char msg[ERR_MSG_MAX_LEN + 1] = { 0 }; \
            snprintf(msg, ERR_MSG_MAX_LEN, __VA_ARGS__); \
            vm_unknown_error(msg); \
        } while (false)


#endif //JVM_JVM_H
