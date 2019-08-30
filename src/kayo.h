/*
 * Author: kayo
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include "jtypes.h"
#include "rtda/heap/mgr/HeapMgr.h"

#ifndef PATH_MAX
#define PATH_MAX 260 // todo
#endif

extern HeapMgr g_heap_mgr;

class ClassLoader;
class StrPool;
struct Thread;


// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
extern std::vector<std::string> jreLibJars;

// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
extern std::vector<std::string> jreExtJars;

/*
 * 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
 *
 * 用户类路径的默认值是当前目录。可以设置CLASSPATH环境变量来修改用户类路径。
 * 可以通过 -cp 选项修改，
 * -cp 选项的优先级更高，可以覆盖CLASSPATH环境变量设置。
 * -cp 选项既可以指定目录，也可以指定JAR文件。
 */
extern std::vector<std::string> userDirs;
extern std::vector<std::string> userJars;

extern StrPool *g_str_pool;

// The system Thread group.
extern Object *sysThreadGroup;

// todo 所有线程
extern std::vector<Thread *> g_all_threads;

/*
 * jvms规定函数最多有255个参数，this也算，long和double占两个长度
 */
#define METHOD_PARAMETERS_MAX_COUNT 255

// name of main thread
#define MAIN_THREAD_NAME "main"

#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

// 出现异常，退出jvm
#define jvm_abort(...) do { printvm("fatal error. "); printf(__VA_ARGS__); exit(-1); } while(false)

/*
 * Virtual Machine Errors
 * A Java Virtual Machine implementation throws an Object that is an instance of
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
void vm_internal_error(const char *msg);

/*
 * java.lang.OutOfMemoryError
 *
 * The Java Virtual Machine implementation has run out of either virtual or physical memory,
 * and the automatic storage mgr was unable to reclaim enough memory to satisfy an Object creation request.
 */
void vm_out_of_memory_error(const char *msg);

/*
 * java.lang.StackOverflowError
 *
 * The Java Virtual Machine implementation has run out of stack space for a Thread,
 * typically because the Thread is doing an unbounded number of recursive invocations
 * as a result of a fault in the executing program.
 */
void vm_stack_overflow_error();

/*
 * java.lang.UnknownError
 *
 * An exception or error has occurred, but the Java Virtual Machine
 * implementation is unable to report the actual exception or error.
 */
void vm_unknown_error(const char *msg);

#define VM_UNKNOWN_ERROR(...) \
        do { \
            char msg[ERR_MSG_MAX_LEN + 1] = { 0 }; \
            snprintf(msg, ERR_MSG_MAX_LEN, __VA_ARGS__); \
            vm_unknown_error(msg); \
        } while (false)


static inline void *vm_malloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
        vm_out_of_memory_error("malloc failed");
    return p;
}

static inline void *vm_calloc(size_t n, size_t size)
{
    void *p = calloc(n, size);
    if (p == NULL)
        vm_out_of_memory_error("malloc failed");
    return p;
}

#endif //JVM_JVM_H
