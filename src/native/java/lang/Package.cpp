#include "../../jnidef.h"

/*
 * Author: Yo Ka
 */

const char *getBootPackage(const char *name);
void *getBootPackages();

// private static native String getSystemPackage0(String name);
static jstring getSystemPackage0(JNIEnv *env, jclass clazz, jstring name)
{
    const char *utf8_name = (*env)->GetStringUTFChars(env, name, NULL);

    const char *pkg = getBootPackage(utf8_name);
    if (pkg == NULL) {
        return NULL;
    } else {
        return  (*env)->NewStringUTF(env, pkg);
    }
}

// private static native String[] getSystemPackages0();
static jobjectArray getSystemPackages0(JNIEnv *env, jclass clazz)
{
    return (jobjectArray) getBootPackages();
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getSystemPackage0", _STR_ STR, (void *) getSystemPackage0},
        { "getSystemPackages0", "()[" STR, (void *) getSystemPackages0 }
};

void java_lang_Package_registerNatives()
{
    registerNatives("java/lang/Package", methods, ARRAY_LENGTH(methods));
}

