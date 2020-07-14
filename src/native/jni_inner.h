/*
 * Author: Yo Ka
 */

#ifndef KAYO_JNI_INNER_H
#define KAYO_JNI_INNER_H

#include "../jvmstd.h"

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
void registerNatives(const char *class_name, JNINativeMethod *methods, int method_count);


#endif //KAYO_JNI_INNER_H
