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

/*
 * 是否开启调试模式(true or false)
 */
#define JVM_DEBUG 0

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


// todo 说明
extern struct classloader *g_bootstrap_loader;

#define CHECK_MALLOC_RESULT(point) do { if ((point) == NULL) vm_internal_error("malloc failed"); } while (false) // todo

/*
 * 下面申请内存的宏会检查申请是否成功
 * 使用时要小心，这些宏展开后是多条语句而不是一条。
 */
#define VM_MALLOC_EXT(type, count, extra_len, var_name) \
                        type *(var_name) = malloc(sizeof(type) * (count) + (extra_len)); \
                        CHECK_MALLOC_RESULT(var_name)

#define VM_MALLOCS(type, count, var_name) VM_MALLOC_EXT(type, count, 0, var_name)

#define VM_MALLOC(type, var_name) VM_MALLOCS(type, 1, var_name)

/*
 * jvms规定函数最多有255个参数，this也算，long和double占两个长度
 */
#define METHOD_PARAMETERS_MAX_COUNT 255


/*
 * 用来表示字段和方法。
 * name_and_type 的名称来源于 Java 虚拟机规范文档.
 * todo 字段的描述符形式，方法的描述符形式
 */
struct name_and_type {
    const char *name;
    const char *descriptor;
};

// The system Thread group.
extern struct jobject *system_thread_group;

// name of main thread
#define MAIN_THREAD_NAME "main"

#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

#if (JVM_DEBUG)
#define printvm_debug(...) printvm(__VA_ARGS__)
#else
#define printvm_debug(...)
#endif

// 出现异常，退出jvm
#define jvm_abort(...) do { printvm("fatal error. "); printf(__VA_ARGS__); exit(-1); } while(false)

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
 * and the automatic storage manager was unable to reclaim enough memory to satisfy an object creation request.
 */
_Noreturn void vm_out_of_memory_error(const char *msg);

/*
 * java.lang.StackOverflowError
 *
 * The Java Virtual Machine implementation has run out of stack space for a thread,
 * typically because the thread is doing an unbounded number of recursive invocations
 * as a result of a fault in the executing program.
 */
_Noreturn void vm_stack_overflow_error(const char *msg);

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
