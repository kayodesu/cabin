/*
 * Author: kayo
 */

#ifndef KAYO_JTYPES_H
#define KAYO_JTYPES_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

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
#define jtrue 1
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


#endif //KAYO_JTYPES_H
