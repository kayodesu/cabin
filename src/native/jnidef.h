/*
 * Author: Yo Ka
 */

#ifndef KAYO_JNI_DEF_H
#define KAYO_JNI_DEF_H

#include "jni.h"

#define OBJ "Ljava/lang/Object;"
#define _OBJ_ "(Ljava/lang/Object;)"

#define CLS "Ljava/lang/Class;"
#define _CLS_ "(Ljava/lang/Class;)"

#define STR "Ljava/lang/String;"
#define _STR_ "(Ljava/lang/String;)"

#define ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(*arr))

static void registerNativesEmptyImplement() { }
#define JNINativeMethod_registerNatives { "registerNatives", "()V", (void *) registerNativesEmptyImplement }

/*
 * 要保证每个 class name 只会注册一次，
 * 重复注册后面注册的无效。
 */
void registerNatives0(const char *class_name, JNINativeMethod *methods, int method_count);


jclass obj_get_class(jobject o);
jobject obj_clone(jobject o);

int is_subclass_of(jclass sub, jclass base);

#endif //KAYO_JNI_DEF_H
