#include "../../jnidef.h"
#include "../../../symbol.h"
#include "../../../vmdef.h"
#include "../../../objects/object.h"

/*
 * 一些帮助函数，只用于本目录下的文件。
 * 
 * Author: Yo Ka
 */

static inline jref __getFileDescriptor(jref _this)
{
    // File Descriptor - handle to the open file
    // private final FileDescriptor fd;
    return _this->getRefField("fd", "Ljava/io/FileDescriptor;");
}

static inline void __openFile(jref _this, jstrref name, const char *mode)
{
    auto utf8_name = name->toUtf8();

    FILE *file = fopen(utf8_name, mode);
    if (file == nullptr) {
        throw FileNotFoundException(utf8_name);
    }

    jref fd = __getFileDescriptor(_this);

    // private long handle;
    fd->setLongField("handle", "J", (jlong) file);
}

static inline FILE *__getFileHandle(jref _this)
{
    jref fd = __getFileDescriptor(_this);

    // private long handle;
//    jclass c = (*env)->GetObjectClass(env, fd);
//    return (FILE *) (*env)->GetLongField(env, fd, (*env)->GetFieldID(env, c, "handle", "J"));
    return (FILE *) fd->getLongField("handle", "J");
}

static inline void __closeFile(jref _this)
{
    FILE *file = __getFileHandle(_this);
    if (fclose(file) != 0) {
        throw IOException();
    }
}