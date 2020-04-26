
#ifndef KAYO_IFN_H
#define KAYO_IFN_H

#include "jni.h"

/*
 * Interfaces For Natives.
 * 封装一些虚拟机功能，供 native methods 调用
 * 
 * Author: Yo Ka
 */
struct IFN {
    jobject (*cloneObject)(jobject o);
    int (*isSubclassOf)(jclass sub, jclass base);
    void (*initClass)(jclass clazz);
    jstring (*intern)(jstring s);

    char *(*dots2SlashDup)(const char *);
    char *(*slash2DotsDup)(const char *);

    // name 以 '/' 分割
    jclass (*loadBootClass)(const char *name);

    // name 以 '/' 分割
    jclass (*loadClass)(jobject loader, const char *name);

    // name 以 '/' 分割
    jclass (*findLoadedClass)(jobject loader, const char *name);

    jclass (*defineClass0)(jobject loader, jstring name, jbyteArray b, jint off, jint len, jobject pd);
    jclass (*defineClass1)(jobject loader, jstring name,
                       jbyteArray b, jint off, jint len, jobject pd, jstring source);
    jclass (*arrayClass)(jclass componentClass);
};

extern struct IFN ifn;

#endif // KAYO_IFN_H