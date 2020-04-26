/*
 * Author: Yo Ka
 */

#ifndef KAYO_JNI_DEF_H
#define KAYO_JNI_DEF_H

#include <stdio.h>
#include <stdlib.h>
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

#ifdef __cplusplus
extern "C" {
#endif
/*
 * 要保证每个 class name 只会注册一次，
 * 重复注册后面注册的无效。
 */
void registerNatives(const char *class_name, JNINativeMethod *methods, int method_count);

#ifdef __cplusplus
}
#endif

#undef printvm
#undef jvm_abort
#define printvm(...) do { printf("%s: %d: ", __FILE__, __LINE__); printf(__VA_ARGS__); } while(false)
// 出现异常，退出jvm
#define jvm_abort(...) do { printvm("fatal error! "); printf(__VA_ARGS__); exit(-1); } while(false)


// jclass obj_get_class(jobject o);
jobject obj_clone(jobject o);

int is_subclass_of(jclass sub, jclass base);

/* C Language Interfaces */

void cli_initClass(jclass clazz);

#endif //KAYO_JNI_DEF_H
