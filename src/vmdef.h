/*
 * Author: kayo
 */

#ifndef JVM_JTYPES_H
#define JVM_JTYPES_H

#include "native/jtypes.h"

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



#endif //JVM_JTYPES_H
