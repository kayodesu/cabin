#include "../../jnidef.h"
#include "../../../symbol.h"

/*
 * 一些帮助函数，只用于本目录下的文件。
 * 
 * Author: Yo Ka
 */

static inline jobject __getFileDescriptor(JNIEnv *env, jobject _this)
{   
    jclass c = (*env)->GetObjectClass(env, _this);

    // File Descriptor - handle to the open file
    // private final FileDescriptor fd;
    jfieldID field = (*env)->GetFieldID(env, c, "fd", "Ljava/io/FileDescriptor;");
    return (*env)->GetObjectField(env, _this, field);
}

static inline void __openFile(JNIEnv *env, jobject _this, jstring name, const char *mode)
{
    const char *utf8_name = (*env)->GetStringUTFChars(env, name, NULL);

    FILE *file = fopen(utf8_name, mode);
    if (file == NULL) {
        jclass c = (*env)->FindClass(env, S(java_io_FileNotFoundException));
        (*env)->ThrowNew(env, c, utf8_name);
    }

    jobject fd = __getFileDescriptor(env, _this);

    // private long handle;
    jclass c = (*env)->GetObjectClass(env, fd);
    (*env)->SetLongField(env, fd, (*env)->GetFieldID(env, c, "handle", "J"), (jlong) file);
}

static inline FILE *__getFileHandle(JNIEnv *env, jobject _this)
{
    jobject fd = __getFileDescriptor(env, _this);

    // private long handle;
    jclass c = (*env)->GetObjectClass(env, fd);
    return (FILE *) (*env)->GetLongField(env, fd, (*env)->GetFieldID(env, c, "handle", "J"));
}

static void __closeFile(JNIEnv *env, jobject _this)
{
    FILE *file = __getFileHandle(env, _this);
    if (fclose(file) != 0) {
        jclass c = (*env)->FindClass(env, S(java_io_IOException));
        (*env)->ThrowNew(env, c, NULL);
    }
}