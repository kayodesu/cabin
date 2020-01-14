/*
 * Author: kayo
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>
#include <dirent.h>
#include "../../registry.h"
#include "../../../runtime/Frame.h"
#include "../../../objects/Object.h"

// private static native void initIDs();
static void initIDs(Frame *frame)
{
    // todo
}

// private native String getDriveDirectory(int drive);
static void getDriveDirectory(Frame *frame)
{
    // todo
    jvm_abort("getDriveDirectory");
}

// private native String canonicalize0(String path) throws IOException;
static void canonicalize0(Frame *frame)
{
    auto so = frame->getLocalAsRef(1);
    // todo 怎么处理路径？？？
    /*
     * 	vars := frame.LocalVars()
	pathStr := vars.GetRef(1)

	// todo
	path := runtime.GoString(pathStr)
	path2 := filepath.Clean(path)
	if path2 != path {
		pathStr = runtime.JString(path2)
	}

	stack := frame.OperandStack()
	stack.PushRef(pathStr)
     */
    frame->pushr(so);
}

// 判断文件或目录是否存在
static inline bool __exist(const char *path)
{
    // todo 这个判断不对，打不开有可能是权限问题或其他问题
    if (open(path, O_RDONLY) != -1 || opendir(path) != NULL) {
        return true;
    }
    return false;
}

// public native int getBooleanAttributes(File f);
static void getBooleanAttributes(Frame *frame)
{
    auto f = frame->getLocalAsRef(1);
    auto path = f->getInstFieldValue<Object *>("path", "Ljava/lang/String;")->toUtf8();

    jint attr = 0;
    if (__exist(path)) {
        attr |= 0x01;

        struct stat buf;
        if (stat(path, &buf) == -1) {
            jvm_abort("stat faild: %s\n", path);  // todo
        }

        if (S_ISDIR(buf.st_mode)) {
            attr |= 0x04;
        }
    }

    frame->pushi(attr);
}

// public native long getLastModifiedTime(File f);
static void getLastModifiedTime(Frame *frame)
{
    // todo
    auto f = frame->getLocalAsRef(1);
    auto path = f->getInstFieldValue<Object *>("path", "Ljava/lang/String;")->toUtf8();

    struct stat buf;
    if (stat(path, &buf) == -1) {
        jvm_abort("stat faild: %s\n", path);  // todo
    }
    frame->pushl(buf.st_mtime);
}

// public native long getLength(File f);
static void getLength(Frame *frame)
{
    // todo
    auto f = frame->getLocalAsRef(1);
    auto path = f->getInstFieldValue<Object *>("path", "Ljava/lang/String;")->toUtf8();

    struct stat buf;
    if (stat(path, &buf) == -1) {
        jvm_abort("stat faild: %s\n", path);  // todo
    }
    frame->pushl(buf.st_size);
}

void java_io_WinNTFileSystem_registerNatives()
{
#undef C
#define C "java/io/WinNTFileSystem"
    registerNative(C, "initIDs", "()V", initIDs);
    registerNative(C, "getDriveDirectory", "(I)Ljava/lang/String;", getDriveDirectory);
    registerNative(C, "canonicalize0", "(Ljava/lang/String;)Ljava/lang/String;", canonicalize0);
    registerNative(C, "getBooleanAttributes", "(Ljava/io/File;)I", getBooleanAttributes);
    registerNative(C, "getLastModifiedTime", "(Ljava/io/File;)J", getLastModifiedTime);
    registerNative(C, "getLength", "(Ljava/io/File;)J", getLength);
}
