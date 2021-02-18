#ifndef CABIN_CABIN_H
#define CABIN_CABIN_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include "symbol.h"

#ifndef PATH_MAX
    #ifdef MAX_PATH
        #define PATH_MAX MAX_PATH
    #else
        #define PATH_MAX 1024
    #endif
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

typedef Object* jref;     // JVM 中的引用类型。
typedef jref    jstrref;  // java.lang.String 的引用。
typedef Array*  jarrref;  // Array 的引用。

typedef Object ClsObj;

#define jnull ((jref) 0)

typedef char utf8_t;
typedef jchar unicode_t;

class Heap;
extern Heap *g_heap;

class ClassLoader;
class Thread;

extern std::string g_java_home;

extern u2 g_classfile_major_version;
extern u2 g_classfile_manor_version;

#define GDK8_CLASSFILE_MAJOR_VERSION 52

#define IS_GDK9_PLUS (g_classfile_major_version > GDK8_CLASSFILE_MAJOR_VERSION)

// The system Thread group.
extern Object *g_sys_thread_group;

// todo 所有线程
extern std::vector<Thread *> g_all_threads;

extern Object *g_app_class_loader;
extern Object *g_platform_class_loader;

extern std::vector<std::pair<const utf8_t *, const utf8_t *>> g_properties;

// jvms规定函数最多有255个参数，this也算，long和double占两个长度
#define METHOD_PARAMETERS_MAX_COUNT 255

// 异常信息最大长度
#define EXCEPTION_MSG_MAX_LEN 1024

#define MAIN_THREAD_NAME "main" // name of main thread
#define GC_THREAD_NAME "gc"     // name of gc thread

#define printvm(...) \
do { \
    printf("%s: %d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
} while(false)

// 出现异常，退出jvm
#define JVM_PANIC(...) \
do { \
    printvm("fatal error! "); \
    printf(__VA_ARGS__); \
    exit(-1); \
} while(false)

// 退出jvm
#define JVM_EXIT exit(0);

#endif //CABIN_CABIN_H
