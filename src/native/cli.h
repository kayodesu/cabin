
#ifndef KAYO_CLI_H
#define KAYO_CLI_H

#include "jni.h"

/*
 * C Language Interfaces. 
 * 封装一些虚拟机功能为C接口，供 native methods 调用
 * 
 * Author: Yo Ka
 */
struct CLI {
    jobject (*cloneObject)(jobject o);
    int (*isSubclassOf)(jclass sub, jclass base);
    void (*initClass)(jclass clazz);
    jstring (*intern)(jstring s);

    // name 以 '.' 分割
    jclass (*loadBootClassDot)(const char *name);

    // name 以 '.' 分割
    jclass (*findLoadedClassDot)(jobject loader, const char *name);

    jclass (*defineClass0)(jobject loader, jstring name, jbyteArray b, jint off, jint len, jobject pd);
    jclass (*defineClass1)(jobject loader, jstring name,
                       jbyteArray b, jint off, jint len, jobject pd, jstring source);
    jclass (*arrayClass)(jclass componentClass);
};

extern struct CLI cli;

#endif // KAYO_CLI_H