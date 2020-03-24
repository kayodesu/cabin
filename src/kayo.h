/*
 * Author: kayo
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include "vmdef.h"
#include "memory/Heap.h"
#include "objects/throwables.h"
#include "symbol.h"

#ifndef PATH_MAX
#define PATH_MAX 1024 // todo
#endif

#define VM_VERSION "1.0" // version of this jvm, a string.
#define JAVA_COMPAT_VERSION "1.8.0_162"
#define MANAGEMENT_VERSION "1.0"

extern Heap g_heap;

class ClassLoader;
//class StrPool;
struct Thread;

extern bool g_jdk_version_9_and_upper;


// 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
extern std::vector<std::string> jreLibJars;

// 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
extern std::vector<std::string> jreExtJars;

extern std::vector<std::string> g_jdk_modules;

/*
 * 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
 *
 * 用户类路径的默认值是当前目录。可以设置CLASSPATH环境变量来修改用户类路径。
 * 可以通过 -cp 选项修改，
 * -cp 选项的优先级更高，可以覆盖CLASSPATH环境变量设置。
 * -cp 选项既可以指定目录，也可以指定JAR文件。
 */
//extern std::vector<std::string> userDirs;
//extern std::vector<std::string> userJars;

//extern char javaHome[];
extern std::string g_java_home;
extern char classpath[];

extern u2 g_classfile_major_version;
extern u2 g_classfile_manor_version;

//extern StrPool *g_str_pool;

// The system Thread group.
extern Object *sysThreadGroup;

// todo 所有线程
extern std::vector<Thread *> g_all_threads;

extern std::vector<std::pair<const utf8_t *, const utf8_t *>> g_properties;

/*
 * jvms规定函数最多有255个参数，this也算，long和double占两个长度
 */
#define METHOD_PARAMETERS_MAX_COUNT 255


#define MAIN_THREAD_NAME "main" // name of main thread
#define GC_THREAD_NAME "gc"

#define MSG_MAX_LEN 1024 // message max length
#define NEW_MSG(...) ({ auto buf = new char[MSG_MAX_LEN]; snprintf(buf, MSG_MAX_LEN, __VA_ARGS__); buf; })

#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)
#define println(...) do { printvm(__VA_ARGS__); printf("\n"); } while(false)

// 出现异常，退出jvm
#define jvm_abort(...) do { printvm("fatal error! "); printf(__VA_ARGS__); exit(-1); } while(false)

#define NEVER_GO_HERE_ERROR(...) do { printvm("never go here! "); jvm_abort(__VA_ARGS__); } while(false)

#endif //JVM_JVM_H
