/*
 * Author: Yo Ka
 */

#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include "../../jni_inner.h"
#include "../../../runtime/frame.h"
#include "../../../objects/object.h"

using namespace std;
using namespace std::filesystem;

// private static native void initIDs();
static void initIDs(JNIEnv *env, jclass clazz)
{
    // todo
}

// private native String getDriveDirectory(int drive);
static void getDriveDirectory(JNIEnv *env, jobject _this, jint drive)
{
    // todo
    jvm_abort("getDriveDirectory");
}

// private native String canonicalize0(String path) throws IOException;
static jstrref canonicalize0(JNIEnv *env, jobject _this, jstrref path)
{
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
    return path;
}

// public native int getBooleanAttributes(File f);
static jint getBooleanAttributes(JNIEnv *env, jobject _this, jref f)
{
    auto __path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

    jint attr = 0;
    path p(__path);
    if (exists(p)) {
        attr |= 0x01;

        directory_entry entry(p);
        if (entry.status().type() == file_type::directory) {
            attr |= 0x04;
        }
    }

    return attr;
}

// public native long getLastModifiedTime(File f);
static jlong getLastModifiedTime(JNIEnv *env, jobject _this, jref f)
{
    // todo
    auto __path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

//    file_time_type mtime = last_write_time(__path);
    struct stat buf; // todo 平台相关代码
    if (stat(__path, &buf) == -1) {
        jvm_abort("stat faild: %s\n", __path);  // todo
    }

    return buf.st_mtime;
}

// public native long getLength(File f);
static jlong getLength(JNIEnv *env, jobject _this, jref f)
{
    // todo
    auto __path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

    uintmax_t size = file_size(__path);
    return size;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initIDs", "()V", (void *) initIDs },
        { "getDriveDirectory", "(I)" STR, (void *) getDriveDirectory },
        { "canonicalize0", "(Ljava/lang/String;)" STR, (void *) canonicalize0 },
        { "getBooleanAttributes", "(Ljava/io/File;)I", (void *) getBooleanAttributes },
        { "getLastModifiedTime", "(Ljava/io/File;)J", (void *) getLastModifiedTime },
        { "getLength", "(Ljava/io/File;)J", (void *) getLength },
};

void java_io_WinNTFileSystem_registerNatives()
{
    registerNatives("java/io/WinNTFileSystem", methods, ARRAY_LENGTH(methods));
}
