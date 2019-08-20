/*
 * Author: kayo
 */

#ifndef JVM_JTYPES_H
#define JVM_JTYPES_H

#include <cstdint>

typedef int8_t  s1;  // s: signed
typedef int16_t s2;
typedef int32_t s4;

typedef uint8_t  u1;  // u: unsigned
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

class Object;

/*
 * Java虚拟机中的整型类型的取值范围如下：
 * 1. 对于byte类型， 取值范围[-2e7, 2e7 - 1]。
 * 2. 对于short类型，取值范围[-2e15, 2e15 - 1]。
 * 3. 对于int类型，  取值范围[-2e31, 2e31 - 1]。
 * 4. 对于long类型， 取值范围[-2e63, 2e63 - 1]。
 * 5. 对于char类型， 取值范围[0, 65535]。
 */
typedef int8_t          jbyte;
typedef jbyte           jbool; // 本虚拟机实现，byte和bool用同一类型
typedef uint16_t        jchar;
typedef int16_t         jshort;
typedef int32_t         jint;
typedef int64_t         jlong;
typedef float          jfloat;
typedef double         jdouble;
typedef Object *        jref; // JVM中的引用类型，只能指向一个object对象。

#define SLOTS(type) ((sizeof(type) + 3)/4) //_Generic((type), jlong: 2, jdouble: 2, default: 1)

static inline jlong i2l(jint i)
{  // todo
    return (jlong) i;
}

static inline jfloat i2f(jint i)
{  // todo
    return (jfloat) i;
}

static inline jdouble i2d(jint i)
{  // todo
    return (jdouble) i;
}

static inline jbool i2z(jint i)
{  // todo
    return (jbool)(i == 0 ? 0 : 1);
}

static inline jbyte i2b(jint i)
{  // todo
    return (jbyte) i;
}

static inline jchar i2c(jint i)
{  // todo 对不对 jchar 是无符号的
    return (jchar) i;
}

static inline jshort i2s(jint i)
{  // todo
    return (jshort) i;
}

static inline jint l2i(jlong l)
{  // todo
    return (jint) l;
}
static inline jfloat l2f(jlong l)
{  // todo
    return (jfloat) l;
}
static inline jdouble l2d(jlong l)
{  // todo
    return (jdouble) l;
}

static inline jint f2i(jfloat f)
{  // todo
    return (jint) f;
}
static inline jlong f2l(jfloat f)
{  // todo
    return (jlong) f;
}
static inline jdouble f2d(jfloat f)
{  // todo
    return (jdouble) f;
}

static inline jint d2i(jdouble d)
{  // todo
    return (jint) d;
}
static inline jlong d2l(jdouble d)
{  // todo
    return (jlong) d;
}
static inline jfloat d2f(jdouble d)
{  // todo
    return (jfloat) d;
}

#endif //JVM_JTYPES_H
