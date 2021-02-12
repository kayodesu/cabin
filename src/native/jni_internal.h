#ifndef CABIN_JNI_INTERNAL_H
#define CABIN_JNI_INTERNAL_H

#include "../cabin.h"
#include "jni.h"

typedef Object* jobject;
typedef Object* jclass;
typedef jobject jstring;

#define OBJ   "Ljava/lang/Object;"
#define _OBJ  "(Ljava/lang/Object;"
#define OBJ_  "Ljava/lang/Object;)"
#define _OBJ_ "(Ljava/lang/Object;)"
#define __OBJ "()Ljava/lang/Object;"

#define CLS   "Ljava/lang/Class;"
#define _CLS  "(Ljava/lang/Class;"
#define CLS_  "Ljava/lang/Class;)"
#define _CLS_ "(Ljava/lang/Class;)"
#define __CLS "()Ljava/lang/Class;"

#define STR   "Ljava/lang/String;"
#define _STR  "(Ljava/lang/String;"
#define STR_  "Ljava/lang/String;)"
#define _STR_ "(Ljava/lang/String;)"
#define __STR "()Ljava/lang/String;"

#define ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(*arr))

#define JNINativeMethod_registerNatives { "registerNatives", "()V", typeid(void(*)()), (void *) (void(*)()) [](){} }

// Type and Address
#undef TA
#define TA(method_name) typeid(&method_name), (void *) method_name

/*
 * 要保证每个 class name 只会注册一次，
 * 重复注册后面注册的无效。
 */
void registerNatives(const char *class_name, JNINativeMethod *methods, int methods_count);


#endif //CABIN_JNI_INTERNAL_H
