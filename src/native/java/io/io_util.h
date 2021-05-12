#include "../../jni_internal.h"
#include "../../../symbol.h"
#include "../../../cabin.h"
#include "../../../objects/object.h"
#include "../../../runtime/vm_thread.h"
#include "../../../exception.h"

static inline jref __getFileDescriptor(jref _this)
{
    // File Descriptor - handle to the open file
    // private final FileDescriptor fd;
    return get_ref_field(_this, "fd", "Ljava/io/FileDescriptor;");
}

static inline void __openFile(jref _this, jstrref name, const char *mode)
{
    utf8_t *utf8_name = string_to_utf8(name);

    FILE *file = fopen(utf8_name, mode);
    if (file == NULL) {
        // throw java_io_FileNotFoundException(std::string(utf8_name) + " (can't find)");
        raise_exception(S(java_io_FileNotFoundException), NULL); // todo msg   
        return;
    }

    jref fd = __getFileDescriptor(_this);

    // private long handle;
    set_long_field(fd, "handle", (jlong) file);
}

static inline FILE *__getFileHandle(jref _this)
{
    jref fd = __getFileDescriptor(_this);

    // private long handle;
//    jclsobj c = (*env)->GetObjectClass(env, fd);
//    return (FILE *) (*env)->GetLongField(env, fd, (*env)->GetFieldID(env, c, "handle", "J"));
    return (FILE *) get_long_field(fd, "handle");
}

static inline void __closeFile(jref _this)
{
    FILE *file = __getFileHandle(_this);
    if (fclose(file) != 0) {
        // throw java_io_IOException();
        raise_exception(S(java_io_IOException), NULL); // todo msg   
    }
}