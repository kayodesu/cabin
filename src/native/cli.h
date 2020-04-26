
#ifndef KAYO_CLI_H
#define KAYO_CLI_H

#include "jni.h"

/*
 * C Language Interfaces. 
 * 封装一些虚拟机功能为C接口，供 native methods 调用
 * 
 * Author: Yo Ka
 */

#ifdef __cplusplus
extern "C" {
#endif

jobject cli_cloneObject(jobject o);

int cli_isSubclassOf(jclass sub, jclass base);

void cli_initClass(jclass clazz);

jstring cli_intern(jstring s);

// name 已 '.' 分割
jclass cli_loadBootClass(const char *name);

// name 已 '.' 分割
jclass cli_findLoadedClass(jobject loader, const char *name);

jclass cli_defineClass0(jobject loader, jstring name, jbyteArray b, jint off, jint len, jobject pd);
jclass cli_defineClass1(jobject loader, jstring name,
                       jbyteArray b, jint off, jint len, jobject pd, jstring source);

jclass cli_arrayClass(jclass componentClass);

#ifdef __cplusplus
}
#endif

#endif // KAYO_CLI_H