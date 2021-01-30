#ifndef CABIN_JNI_INNER_H
#define CABIN_JNI_INNER_H

#include "../jvmstd.h"

/*
 * NOTICE:
 * 不能再native函数中用throw抛异常，libffi不能处理这种情况，
 * 会导致jvm假死（不会崩溃退出，进程还在，但不工作，CUP利用率近乎零）。
 */

class ClassObject;

typedef Object*      jobject;
typedef ClassObject* jclass;
typedef jobject      jthrowable;
typedef jobject      jstring;
typedef Array*       jarray;
typedef jarray       jbooleanArray;
typedef jarray       jbyteArray;
typedef jarray       jcharArray;
typedef jarray       jshortArray;
typedef jarray       jintArray;
typedef jarray       jlongArray;
typedef jarray       jfloatArray;
typedef jarray       jdoubleArray;
typedef jarray       jobjectArray;


#define OBJ "Ljava/lang/Object;"
#define _OBJ_ "(Ljava/lang/Object;)"
#define _OBJ "(Ljava/lang/Object;"
#define OBJ_ "Ljava/lang/Object;)"

#define CLS "Ljava/lang/Class;"
#define _CLS "(Ljava/lang/Class;"
#define _CLS_ "(Ljava/lang/Class;)"

#define STR "Ljava/lang/String;"
#define _STR_ "(Ljava/lang/String;)"

#define ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(*arr))

#define JNINativeMethod_registerNatives { "registerNatives", "()V", (void *) (void(*)()) [](){} }

struct JNINativeMethod {
    const char *name;
    const char *descriptor;
    void *func;
};

/*
 * 要保证每个 class name 只会注册一次，
 * 重复注册后面注册的无效。
 */
void registerNatives(const char *class_name, JNINativeMethod *methods, int methods_count);


#endif //CABIN_JNI_INNER_H
