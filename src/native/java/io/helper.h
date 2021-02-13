#include "../../jni_internal.h"
#include "../../../symbol.h"
#include "../../../cabin.h"
#include "../../../objects/object.h"
#include "../../../runtime/vm_thread.h"
#include "../../../exception.h"

/*
 * 一些帮助函数，只用于本目录下的文件。
 */

static inline jobject __getFileDescriptor(jobject _this)
{
    // File Descriptor - handle to the open file
    // private final FileDescriptor fd;
    return _this->getRefField("fd", "Ljava/io/FileDescriptor;");
}

static inline void __openFile(jobject _this, jstring name, const char *mode)
{
    auto utf8_name = name->toUtf8();

    FILE *file = fopen(utf8_name, mode);
    if (file == nullptr) {
        throw java_io_FileNotFoundException(std::string(utf8_name) + " (can't find)");
        return;
    }

    jobject fd = __getFileDescriptor(_this);

    // private long handle;
    fd->setLongField("handle", "J", (jlong) file);
}

static inline FILE *__getFileHandle(jobject _this)
{
    jobject fd = __getFileDescriptor(_this);

    // private long handle;
//    jclass c = (*env)->GetObjectClass(env, fd);
//    return (FILE *) (*env)->GetLongField(env, fd, (*env)->GetFieldID(env, c, "handle", "J"));
    return (FILE *) fd->getLongField("handle", "J");
}

static inline void __closeFile(jobject _this)
{
    FILE *file = __getFileHandle(_this);
    if (fclose(file) != 0) {
        throw java_io_IOException();
    }
}