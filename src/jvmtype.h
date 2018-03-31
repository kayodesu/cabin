/*
 * Author: Jia Yang
 */

#ifndef JVM_JVMTYPE_H
#define JVM_JVMTYPE_H

#include<string>

typedef int8_t s1;  // s: signed
typedef int16_t s2;
//typedef int32_t s4;

typedef uint8_t u1;  // u: unsigned
typedef uint16_t u2;
typedef uint32_t u4;
//typedef uint64_t u8;

/*
 * Java虚拟机中的整型类型的取值范围如下：
 * 1. 对于byte类型，取值范围是从-128至127（-2e7至2e7 - 1），包括-128和127。
 * 2. 对于short类型，取值范围是从−32768至32767（-2e15至2e15 - 1），包括−32768和32767。
 * 3. 对于int类型，取值范围是从−2147483648至2147483647（-2e31至2e31 - 1），包括−2147483648和2147483647。
 * 4. 对于long类型，取值范围是从−9223372036854775808至9223372036854775807（-2e63至2e63 - 1），
 *    包括−9223372036854775808和9223372036854775807。
 * 5. 对于char类型，取值范围是从0至65535，包括0和65535。
 */
typedef int8_t   jbyte;
typedef jbyte    jbool; // 本虚拟机实现，byte和bool用同一类型
typedef uint16_t jchar;
typedef int16_t  jshort;
typedef int32_t  jint;
typedef int64_t  jlong;
typedef float   jfloat;
typedef double  jdouble;
typedef void *  jreference;
typedef std::u16string jstring;

#define jtrue ((jbool)1)
#define jfalse ((jbool)0)

// 不要改变顺序
enum Jtype {
    PRIMITIVE_BOOLEAN,
    PRIMITIVE_BYTE,
    PRIMITIVE_CHAR,
    PRIMITIVE_SHORT,
    PRIMITIVE_INT,
    PRIMITIVE_FLOAT,
    REFERENCE,

    PRIMITIVE_LONG,
    PRIMITIVE_DOUBLE,

    INVALID_JVM_TYPE,
};

// 不要改变顺序
static const char *jtypeNames[] {
    "boolean", "byte", "char", "short", "int", "float", "reference", "long", "double", "invalid jvm type"
};

union Jvalue {
    jbool z;
    jbyte b;
    jchar c;
    jshort s;
    jint i;
    jfloat f;
    jlong l;
    jdouble d;
    jreference r;
};


#endif //JVM_JVMTYPE_H
