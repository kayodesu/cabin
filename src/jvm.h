/*
 * Author: Jia Yang
 */

#ifndef JVM_JVM_H
#define JVM_JVM_H

#include <string>
//#include "loader/ClassLoader.h"
//#include "rtda/heap/objectarea/Jobject.h"
class Jobject;
class ClassLoader;

extern ClassLoader *classLoader;
extern Jobject *mainThreadGroup;

void startJVM(const std::string &mainClassName);


#define jprintf(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)

// 出现异常，退出jvm
#define jvmAbort(...) do { jprintf("fatal error. "); printf(__VA_ARGS__); exit(-1); } while(false)

struct JvmEnv {
    // 启动类路径（bootstrap classpath）默认对应 jre/lib 目录，Java标准库（大部分在rt.jar里）位于该路径
    static std::string bootstrapClasspath;

    // 扩展类路径（extension classpath）默认对应 jre/lib/ext 目录，使用Java扩展机制的类位于这个路径。
    static std::string extensionClasspath;

    // 用户类路径（user classpath）我们自己实现的类，以及第三方类库位于用户类路径
    static std::string userClasspath;

private:
    JvmEnv() = default;
};


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
// 与 Jtype 顺序对应
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



#endif //JVM_JVM_H
