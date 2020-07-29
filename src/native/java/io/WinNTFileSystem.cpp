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
static void initIDs()
{
    // todo
}

// private native String getDriveDirectory(int drive);
static void getDriveDirectory(jobject _this, jint drive)
{
    // todo
    jvm_abort("getDriveDirectory");
}

// private native String canonicalize0(String path) throws IOException;
static jstring canonicalize0(jobject _this, jstring path)
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

/*
 * Run the canonicalization operation assuming that the prefix
 * (everything up to the last filename) is canonical; just gets
 * the canonical name of the last element of the path
 *
 * private native String canonicalizeWithPrefix0(String canonicalPrefix, String pathWithCanonicalPrefix) throws IOException;
 */
static jlong canonicalizeWithPrefix0(jobject _this, jstring canonicalPrefix, jstring pathWithCanonicalPrefix)
{
    jvm_abort("canonicalizeWithPrefix0");
}

// public native int getBooleanAttributes(File f);
static jint getBooleanAttributes(jobject _this, jobject f)
{
    auto _path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

    jint attr = 0;
    path p(_path);
    if (exists(p)) {
        attr |= 0x01;

        directory_entry entry(p);
        if (entry.status().type() == file_type::directory) {
            attr |= 0x04;
        }
    }

    return attr;
}

// public native boolean checkAccess(File f, int access);
static jboolean checkAccess(jobject _this, jobject f, jint access)
{
    jvm_abort("checkAccess");
}

// public native long getLastModifiedTime(File f);
static jlong getLastModifiedTime(jobject _this, jobject f)
{
    // todo
    auto _path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

//    file_time_type mtime = last_write_time(__path);
    struct stat buf; // todo 平台相关代码
    if (stat(_path, &buf) == -1) {
        jvm_abort("stat faild: %s\n", _path);  // todo
    }

    return buf.st_mtime;
}

// public native long getLength(File f);
static jlong getLength(jobject _this, jobject f)
{
    // todo
    auto _path = f->getRefField("path", "Ljava/lang/String;")->toUtf8();

    uintmax_t size = file_size(_path);
    return size;
}

// public native boolean setPermission(File f, int access, boolean enable, boolean ownerOnly);
static jboolean setPermission(jobject _this, jobject f, jint access, jboolean enable, jboolean owner_only)
{
    jvm_abort("setPermission");
}

// public native boolean createFileExclusively(String path) throws IOException;
static jboolean createFileExclusively(jobject _this, jstring path)
{
    jvm_abort("createFileExclusively");
}

// public native String[] list(File f);
static jobjectArray list(jobject _this, jobject f)
{
    jvm_abort("list");
}

// public native boolean createDirectory(File f);
static jboolean createDirectory(jobject _this, jobject f)
{
    jvm_abort("createDirectory");
}

// public native boolean setLastModifiedTime(File f, long time);
static jboolean setLastModifiedTime(jobject _this, jobject f, jlong time)
{
    jvm_abort("setLastModifiedTime");
}

// public native boolean setReadOnly(File f);
static jboolean setReadOnly(jobject _this, jobject f)
{
    jvm_abort("setReadOnly");
}

// private native boolean delete0(File f);
static jboolean delete0(jobject _this, jobject f)
{
    jvm_abort("delete0");
}

// private native boolean rename0(File f1, File f2);
static jboolean rename0(jobject _this, jobject f1, jobject f2)
{
    jvm_abort("rename0");
}

// private static native int listRoots0();
static jint listRoots0(jobject _this)
{
    jvm_abort("listRoots0");
}

// private native long getSpace0(File f, int t);
static jlong getSpace0(jobject _this, jobject f1, jint t)
{
    jvm_abort("getSpace0");
}

/*
 * Obtain maximum file component length from GetVolumeInformation which
 * expects the path to be null or a root component ending in a backslash
 *
 * private native int getNameMax0(String path);
 */
static jint getNameMax0(jobject _this, jstring path)
{
    jvm_abort("getNameMax0");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initIDs", "()V", (void *) initIDs },
        { "getDriveDirectory", "(I)" STR, (void *) getDriveDirectory },
        { "canonicalize0", "(Ljava/lang/String;)" STR, (void *) canonicalize0 },
        { "canonicalizeWithPrefix0", "(" STR STR ")" STR, (void *) canonicalizeWithPrefix0 },
        { "getBooleanAttributes", "(Ljava/io/File;)I", (void *) getBooleanAttributes },
        { "checkAccess", "(Ljava/io/File;I)Z", (void *) checkAccess },
        { "getLastModifiedTime", "(Ljava/io/File;)J", (void *) getLastModifiedTime },
        { "getLength", "(Ljava/io/File;)J", (void *) getLength },
        { "setPermission", "(Ljava/io/File;IZZ)Z", (void *) setPermission },
        { "createFileExclusively", "(Ljava/lang/String;)Z", (void *) createFileExclusively },
        { "list", "(Ljava/io/File;)[Ljava/lang/String;", (void *) list },
        { "createDirectory", "(Ljava/io/File;)Z", (void *) createDirectory },
        { "setLastModifiedTime", "(Ljava/io/File;J)Z", (void *) setLastModifiedTime },
        { "setReadOnly", "(Ljava/io/File;)Z", (void *) setReadOnly },
        { "delete0", "(Ljava/io/File;)Z", (void *) delete0 },
        { "rename0", "(Ljava/io/File;Ljava/io/File;)Z", (void *) rename0 },
        { "listRoots0", "()I", (void *) listRoots0 },
        { "getSpace0", "(Ljava/io/File;I)J", (void *) getSpace0 },
        { "getNameMax0", "(Ljava/lang/String;)I", (void *) getNameMax0 },
};

void java_io_WinNTFileSystem_registerNatives()
{
    registerNatives("java/io/WinNTFileSystem", methods, ARRAY_LENGTH(methods));
}
