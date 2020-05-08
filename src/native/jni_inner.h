/*
 * Author: Yo Ka
 */

#ifndef KAYO_JNI_INNER_H
#define KAYO_JNI_INNER_H

#include <vector>
#include "../objects/class.h"
#include "../objects/field.h"
#include "../objects/method.h"
#include "../objects/class_loader.h"
#include "../interpreter/interpreter.h"


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

static void registerNativesEmptyImplement() { }
#define JNINativeMethod_registerNatives { "registerNatives", "()V", (void *) registerNativesEmptyImplement }

struct JNINativeMethod {
    const char *name;
    const char *signature;
    void *func;
};

/*
 * 要保证每个 class name 只会注册一次，
 * 重复注册后面注册的无效。
 */
void registerNatives(const char *class_name, JNINativeMethod *methods, int method_count);


#endif //KAYO_JNI_INNER_H
