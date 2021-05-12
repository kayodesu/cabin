// #include <fstream>
// #include <filesystem>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../../jni_internal.h"
#include "../../../runtime/frame.h"
#include "../../../objects/object.h"

// using namespace std;
// using namespace std::filesystem;

// private static native void initIDs();
static void initIDs()
{
    // todo
}

// private native String getDriveDirectory(int drive);
static void getDriveDirectory(jobject _this, jint drive)
{
    // todo
    JVM_PANIC("getDriveDirectory");
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
    JVM_PANIC("canonicalizeWithPrefix0");
}

// public native int getBooleanAttributes(File f);
static jint getBooleanAttributes(jobject _this, jobject f)
{
    utf8_t *path = jstring_to_utf8(get_ref_field(f, "path", "Ljava/lang/String;"));

    jint attr = 0;

    struct stat buf; 
    if (stat(path, &buf) == 0) {
        attr |= 0x01;
        if (S_IFDIR & buf.st_mode) { 
            attr |= 0x04;
        }
    }

    // path p(_path);
    // if (exists(p)) {
    //     attr |= 0x01;

    //     directory_entry entry(p);
    //     if (entry.status().type() == file_type::directory) {
    //         attr |= 0x04;
    //     }
    // }

    return attr;
}

// public native boolean checkAccess(File f, int access);
static jboolean checkAccess(jobject _this, jobject f, jint access)
{
    JVM_PANIC("checkAccess");
}

// public native long getLastModifiedTime(File f);
static jlong getLastModifiedTime(jobject _this, jobject f)
{
    // todo
    auto path = jstring_to_utf8(get_ref_field(f, "path", "Ljava/lang/String;"));

//    file_time_type mtime = last_write_time(__path);
    struct stat buf;  
    if (stat(path, &buf) == -1) {
        JVM_PANIC("stat faild: %s\n", path);  // todo
    }

    return buf.st_mtime;
}

// public native long getLength(File f);
static jlong getLength(jobject _this, jobject f)
{
    // todo
    utf8_t *path = jstring_to_utf8(get_ref_field(f, "path", "Ljava/lang/String;"));

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        // todo error
        JVM_PANIC(path);  // todo
    }
    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);

    // uintmax_t size = file_size(_path);
    return file_len;
}

// public native boolean setPermission(File f, int access, boolean enable, boolean ownerOnly);
static jboolean setPermission(jobject _this, jobject f, jint access, jboolean enable, jboolean owner_only)
{
    JVM_PANIC("setPermission");
}

// public native boolean createFileExclusively(String path) throws IOException;
static jboolean createFileExclusively(jobject _this, jstring path)
{
    JVM_PANIC("createFileExclusively");
}

// public native String[] list(File f);
static jobject list(jobject _this, jobject f)
{
    JVM_PANIC("list");
}

// public native boolean createDirectory(File f);
static jboolean createDirectory(jobject _this, jobject f)
{
    JVM_PANIC("createDirectory");
}

// public native boolean setLastModifiedTime(File f, long time);
static jboolean setLastModifiedTime(jobject _this, jobject f, jlong time)
{
    JVM_PANIC("setLastModifiedTime");
}

// public native boolean setReadOnly(File f);
static jboolean setReadOnly(jobject _this, jobject f)
{
    JVM_PANIC("setReadOnly");
}

// private native boolean delete0(File f);
static jboolean delete0(jobject _this, jobject f)
{
    JVM_PANIC("delete0");
}

// private native boolean rename0(File f1, File f2);
static jboolean rename0(jobject _this, jobject f1, jobject f2)
{
    JVM_PANIC("rename0");
}

// private static native int listRoots0();
static jint listRoots0(jobject _this)
{
    JVM_PANIC("listRoots0");
}

// private native long getSpace0(File f, int t);
static jlong getSpace0(jobject _this, jobject f1, jint t)
{
    JVM_PANIC("getSpace0");
}

/*
 * Obtain maximum file component length from GetVolumeInformation which
 * expects the path to be null or a root component ending in a backslash
 *
 * private native int getNameMax0(String path);
 */
static jint getNameMax0(jobject _this, jstring path)
{
    JVM_PANIC("getNameMax0");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initIDs", "()V", TA(initIDs) },
        { "getDriveDirectory", "(I)" STR, TA(getDriveDirectory) },
        { "canonicalize0", _STR_ STR, TA(canonicalize0) },
        { "canonicalizeWithPrefix0", _STR STR_ STR, TA(canonicalizeWithPrefix0) },
        { "getBooleanAttributes", "(Ljava/io/File;)I", TA(getBooleanAttributes) },
        { "checkAccess", "(Ljava/io/File;I)Z", TA(checkAccess) },
        { "getLastModifiedTime", "(Ljava/io/File;)J", TA(getLastModifiedTime) },
        { "getLength", "(Ljava/io/File;)J", TA(getLength) },
        { "setPermission", "(Ljava/io/File;IZZ)Z", TA(setPermission) },
        { "createFileExclusively", _STR_ "Z", TA(createFileExclusively) },
        { "list", "(Ljava/io/File;)[" STR, TA(list) },
        { "createDirectory", "(Ljava/io/File;)Z", TA(createDirectory) },
        { "setLastModifiedTime", "(Ljava/io/File;J)Z", TA(setLastModifiedTime) },
        { "setReadOnly", "(Ljava/io/File;)Z", TA(setReadOnly) },
        { "delete0", "(Ljava/io/File;)Z", TA(delete0) },
        { "rename0", "(Ljava/io/File;Ljava/io/File;)Z", TA(rename0) },
        { "listRoots0", "()I", TA(listRoots0) },
        { "getSpace0", "(Ljava/io/File;I)J", TA(getSpace0) },
        { "getNameMax0", _STR_ "I", TA(getNameMax0) },
};

void java_io_WinNTFileSystem_registerNatives()
{
    registerNatives("java/io/WinNTFileSystem", methods, ARRAY_LENGTH(methods));
}
