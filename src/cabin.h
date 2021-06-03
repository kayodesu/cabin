#ifndef CABIN_CABIN_H
#define CABIN_CABIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#ifndef PATH_MAX
    #ifdef MAX_PATH
        #define PATH_MAX MAX_PATH
    #else
        #define PATH_MAX 1024
    #endif
#endif

#define VM_VERSION "1.0" // version of this jvm, a string.
#define JAVA_COMPAT_VERSION "1.8.0_162"

// jvm 最大支持的classfile版本
#define JVM_MUST_SUPPORT_CLASSFILE_MAJOR_VERSION 60
#define JVM_MUST_SUPPORT_CLASSFILE_MINOR_VERSION 65535

// size of heap
#define VM_HEAP_SIZE (512*1024*1024) // 512Mb

// every thread has a vm stack
#define VM_STACK_SIZE (512*1024)     // 512Kb

// jni 局部引用表默认最大容量 
#define JNI_LOCAL_REFERENCE_TABLE_MAX_CAPACITY 512

// jvm 最大支持的线程数量
#define VM_THREADS_MAX_COUNT 65535

/*
 * Java虚拟机中的整型类型的取值范围如下：
 * 1. 对于byte类型， 取值范围[-2e7, 2e7 - 1]。
 * 2. 对于short类型，取值范围[-2e15, 2e15 - 1]。
 * 3. 对于int类型，  取值范围[-2e31, 2e31 - 1]。
 * 4. 对于long类型， 取值范围[-2e63, 2e63 - 1]。
 * 5. 对于char类型， 取值范围[0, 65535]。
 */
typedef int8_t   jbyte;
typedef jbyte    jboolean; // 本虚拟机实现，byte 和 boolean 用同一类型
typedef jboolean jbool;
typedef uint16_t jchar;
typedef int16_t  jshort;
typedef int32_t  jint;
typedef int64_t  jlong;
typedef float    jfloat;
typedef double   jdouble;

#define jtrue  1
#define jfalse 0

typedef jint jsize;

#define JINT_TO_JBOOL(_i)  ((_i) != 0 ? true : false)
#define JINT_TO_JBYTE(_i)  ((jbyte)((_i) & 0xff))
#define JINT_TO_JCHAR(_i)  ((jchar)((_i) & 0xffff))
#define JINT_TO_JSHORT(_i) ((jshort)((_i) & 0xffff))

typedef int8_t  s1;  // s: signed
typedef int16_t s2;
typedef int32_t s4;

typedef uint8_t  u1; // u: unsigned
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

#define U2_MAX 65535

typedef struct object Object;
typedef struct class  Class;
typedef struct method Method;
typedef struct field  Field;

typedef Object* jref; // JVM 中的引用类型。
typedef jref jstrRef; // java.lang.String 的引用。
typedef jref jarrRef; // Array 的引用。
typedef jref jclsRef; // java.lang.Class 的引用。

typedef char utf8_t;
typedef jchar unicode_t;

struct heap;
extern struct heap *g_heap;

extern pthread_mutexattr_t g_pthread_mutexattr_recursive;

extern char g_java_home[];

extern u2 g_classfile_major_version;
extern u2 g_classfile_manor_version;

// The system Thread group.
extern Object *g_sys_thread_group;

struct vm_thread;
struct point_hash_map;
struct point_hash_set;

// todo 所有线程
extern struct vm_thread *g_all_threads[];
extern int g_all_threads_count;
#define add_thread(t) g_all_threads[g_all_threads_count++] = t

#define BOOT_CLASS_LOADER NULL
extern Object *g_app_class_loader;
extern Object *g_platform_class_loader;

extern struct property {
    const utf8_t *name;
    const utf8_t *value;
} g_properties[];

#define PROPERTIES_MAX_COUNT 128
extern int g_properties_count;

typedef struct {

} InitArgs;

// jvms规定函数最多有255个参数，this也算，long和double占两个长度
#define METHOD_PARAMETERS_MAX_COUNT 255

// jvms数组最大维度为255 
#define ARRAY_MAX_DIMENSIONS 255

#define INFO_MSG_MAX_LEN 1024

#define MAIN_THREAD_NAME "main" // name of main thread
#define GC_THREAD_NAME "gc"     // name of gc thread

#define vm_malloc malloc
#define vm_calloc(len) calloc(1, len)
#define vm_realloc realloc

// Mark a method may throw java exception.
// The caller of this method must check the may throw java exception.
#define TJE // Throw Java Exception

#define ARRAY_LENGTH(_arr_) (sizeof(_arr_)/sizeof(*(_arr_))) 

#define BUILD_ARRAY(_arr, _len, _init_func, ...) \
do { \
    (_arr) = vm_malloc((_len) * sizeof(*(_arr))); \
    for (int _i = 0; _i < (_len); _i++) { \
        _init_func((_arr) + _i, __VA_ARGS__); \
    } \
} while(false)

#define BUILD_ARRAY0(_arr, _len, _rvalue) \
do { \
    (_arr) = vm_malloc((_len) * sizeof(*(_arr))); \
    for (int _i = 0; _i < (_len); _i++) { \
        (_arr)[_i] = _rvalue; \
    } \
} while(false)

#define printvm(...) \
do { \
    printf("%s: %d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
} while(false)

#define println(...) do { printvm(__VA_ARGS__); printf("\n"); } while(0)

/* ------- 配置日志级别 ------ */
// 0: ERR
// 1: WARNING
// 2: DEBUG
// 3: TRACE
// 4: VERBOSE
#define LOG_LEVEL 0

#define ERR     println
#define WARN    println
#define DEBUG   println
#define TRACE   println
#define VERBOSE println

#if (LOG_LEVEL < 4)
#undef VERBOSE
#define VERBOSE(...)
#endif

#if (LOG_LEVEL < 3)
#undef TRACE
#define TRACE(...)
#endif

#if (LOG_LEVEL < 2)
#undef DEBUG
#define DEBUG(...)
#endif

#if (LOG_LEVEL < 1)
#undef WARNING
#define WARNING(...)
#endif

// 出现异常，退出jvm
#define JVM_PANIC(...) \
do { \
    printvm("fatal error! "); \
    printf(__VA_ARGS__); \
    exit(-1); \
} while(false)

#define SHOULD_NEVER_REACH_HERE(...) \
do { \
    printvm("should never reach here. "); \
    printf(__VA_ARGS__); \
    exit(-1); \
} while(false)

// 退出jvm
#define JVM_EXIT exit(0);

#endif //CABIN_CABIN_H
