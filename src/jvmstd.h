/*
 * Author: Yo Ka
 */

#ifndef JVM_JVMSTD_H
#define JVM_JVMSTD_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include "symbol.h"

#ifndef PATH_MAX
#define PATH_MAX 1024 // todo
#endif

#define VM_VERSION "1.0" // version of this jvm, a string.
#define JAVA_COMPAT_VERSION "1.8.0_162"
#define MANAGEMENT_VERSION "1.0"

/*
 * Java虚拟机中的整型类型的取值范围如下：
 * 1. 对于byte类型， 取值范围[-2e7, 2e7 - 1]。
 * 2. 对于short类型，取值范围[-2e15, 2e15 - 1]。
 * 3. 对于int类型，  取值范围[-2e31, 2e31 - 1]。
 * 4. 对于long类型， 取值范围[-2e63, 2e63 - 1]。
 * 5. 对于char类型， 取值范围[0, 65535]。
 */
typedef int8_t          jbyte;
typedef jbyte           jboolean; // 本虚拟机实现，byte 和 boolean 用同一类型
typedef uint16_t        jchar;
typedef int16_t         jshort;
typedef int32_t         jint;
typedef int64_t         jlong;
typedef float           jfloat;
typedef double          jdouble;

typedef jboolean   jbool;
#define jtrue  1
#define jfalse 0

static inline bool is_jfalse(jbool b)
{
    return b == jfalse;
}

static inline bool is_jtrue(jbool b)
{
    // 不能用 b == jtrue 判断，
    // 非 jfalse 就为 jtrue
    return !is_jfalse(b);
}

typedef jint jsize;

static inline jbool jint2jbool(jint i)
{
    return i != 0 ? jtrue : jfalse;
}

static inline jbyte jint2jbyte(jint i)
{
    return (jbyte)(i & 0xff);
}

static inline jchar jint2jchar(jint i)
{
    return (jchar)(i & 0xffff);
}

static inline jshort jint2jshort(jint i)
{
    return (jshort)(i & 0xffff);
}

typedef int8_t  s1;  // s: signed
typedef int16_t s2;
typedef int32_t s4;

typedef uint8_t  u1; // u: unsigned
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

#define U2_MAX 65535

class Object;
class Array;
class Class;

typedef Object*         jref;     // JVM中的引用类型。
typedef Object*         jprimref; // JVM中基本类型的包装类的引用。
typedef Object*         jstrref;  // java.lang.String 的引用。
typedef Array*          jarrref;  // JVM中 Array 的引用
typedef Class*          jclsref;  // java.lang.Class 的引用


#define jnull       ((jref) 0)

typedef char utf8_t;
typedef jchar unicode_t;

class Heap;
extern Heap *g_heap;

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

// 出现异常，退出jvm
#define jvm_abort(...) do { printvm("fatal error! "); printf(__VA_ARGS__); exit(-1); } while(false)

#endif //JVM_JVMSTD_H
