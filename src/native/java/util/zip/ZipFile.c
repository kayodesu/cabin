#include <minizip/unzip.h>
#include "../../../jni_internal.h"
#include "../../../../symbol.h"
#include "../../../../cabin.h"
#include "../../../../objects/object.h"
#include "../../../../runtime/vm_thread.h"
#include "../../../../exception.h"

// private static native void initIDs();
void initIDs()
{
    // todo
}

// private static native long open(String name, int mode, long lastModified, boolean usemmap) throws IOException;
jlong __open(jstring name, jint mode, jlong lastModified, jboolean usemmap)
{
    const char *utf8 = jstring_to_utf8(name);

    // todo 其他几个参数怎么搞？？
    unzFile jzfile = unzOpen64(utf8);
    if (jzfile == NULL) {
        // throw java_io_IOException(utf8);
        raise_exception(S(java_io_IOException), NULL); // todo msg   
    }

    return (jlong) (jzfile);
}

// private static native boolean startsWithLOC(long jzfile);
jboolean startsWithLOC(jlong jzfile)
{
    // todo
    return jtrue;
}

// private static native int getTotal(long jzfile);
jint getTotal(jlong jzfile)
{
    unz_global_info64 info;
    if (unzGetGlobalInfo64((unzFile) jzfile, &info) != UNZ_OK) {
        // todo error
        JVM_PANIC("unzGetGlobalInfo64 failed\n");
    }
    return info.number_entry;
}

// private static native long getNextEntry(long jzfile, int i);
jlong getNextEntry(jlong jzfile, jint i)
{
/*
 * 	vars := frame.LocalVars()
	jzfile := vars.GetLong(0)
	i := vars.GetInt(2)

	jzentry := getJzentry(jzfile, i)

	stack := frame.OperandStack()
	stack.PushLong(jzentry)
 */
    // todo
    JVM_PANIC("getNextEntry");
}

// // private static native void freeEntry(long jzfile, long jzentry);
void freeEntry(jlong jzfile, jlong jzentry)
{
    // todo
}

// private static native long getEntry(long jzfile, byte[] name, boolean addSlash);
jlong getEntry(jlong jzfile, jobject name, jboolean addSlash)
{
    // todo
    /*
     * 	vars := frame.LocalVars()
	jzfile := vars.GetLong(0)
	nameObj := vars.GetRef(2)
	//addSlash := vars.GetBoolean(3)

	// todo
	name := nameObj.GoBytes()
	jzentry := getJzentry2(jzfile, name)

	stack := frame.OperandStack()
	stack.PushLong(jzentry)
     */
    JVM_PANIC("getEntry");
}

// private static native byte[] getEntryBytes(long jzentry, int type);
jobject getEntryBytes(jlong jzentry, jint type)
{
    JVM_PANIC("getEntryBytes");
// todo
    /*
     * 	vars := frame.LocalVars()
	jzentry := vars.GetLong(0)
	_type := vars.GetInt(2)

	goBytes := _getEntryBytes(jzentry, _type)
	jBytes := jutil.CastUint8sToInt8s(goBytes)
	byteArr := heap.NewByteArray(jBytes)

	stack := frame.OperandStack()
	stack.PushRef(byteArr)
     */

    /*
    func _getEntryBytes(jzentry int64, _type int32) []byte {
	entry := getEntryFile(jzentry)
	switch _type {
	case JZENTRY_NAME:
		return []byte(entry.Name)
	case JZENTRY_EXTRA:
		return entry.Extra
	case JZENTRY_COMMENT:
		return []byte(entry.Comment)
	}
	jutil.Panicf("BAD type: %v", _type)
	return nil
}
     */
}

// private static native int getEntryFlag(long jzentry);
jint getEntryFlag(jlong jzentry)
{
    JVM_PANIC("getEntryFlag");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	flag := int32(entry.Flags)

	stack := frame.OperandStack()
	stack.PushInt(flag)
 */
}

// private static native long getEntryTime(long jzentry);
jlong getEntryTime(jlong jzentry)
{
    JVM_PANIC("getEntryTime");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	modDate := entry.ModifiedDate
	modTime := entry.ModifiedTime
	time := int64(modDate)<<16 | int64(modTime)

	stack := frame.OperandStack()
	stack.PushLong(time)
 */
}

// private static native long getEntryCrc(long jzentry);
jlong getEntryCrc(jlong jzentry)
{
    JVM_PANIC("getEntryCrc");
    /*
     * todo
     * 	entry := _getEntryPop(frame)
	crc := int64(entry.CRC32)

	stack := frame.OperandStack()
	stack.PushLong(crc)
     */
}

// private static native long getEntrySize(long jzentry);
jlong getEntrySize(jlong jzentry)
{
    JVM_PANIC("getEntrySize");
/*
 * todo
 * 	entry := _getEntryPop(frame)
	size := int64(entry.UncompressedSize64)

	stack := frame.OperandStack()
	stack.PushLong(size)
 */
}

// private static native long getEntryCSize(long jzentry);
jlong getEntryCSize(jlong jzentry)
{
    JVM_PANIC("getEntryCSize");
    /*
     * todo
     *
     * 	// entry := _getEntryPop(frame)
	// size := int64(entry.CompressedSize64)

	// stack := frame.OperandStack()
	// stack.PushLong(size)

	// todo
	getEntrySize(frame)

     */
}

// private static native int getEntryMethod(long jzentry);
jint getEntryMethod(jlong jzentry)
{
    JVM_PANIC("getEntryMethod");
    /*
     * todo
     * 	// entry := _getEntryPop(frame)
	// method := int32(entry.Method)

	// todo
	stack := frame.OperandStack()
	stack.PushInt(0)
     */

    /*
     * func _getEntryPop(frame *runtime.Frame) *gozip.File {
	vars := frame.LocalVars()
	jzentry := vars.GetLong(0)

	entry := getEntryFile(jzentry)
	return entry
}
     */
}

// private static native int read(long jzfile, long jzentry, long pos, byte[] b, int off, int len);
jint __read(jlong jzfile, jlong jzentry, jlong pos, jobject b, jint off, jint len)
{
    JVM_PANIC("read");
/*
 * todo
 * 	vars := frame.LocalVars()
	//jzfile := vars.GetLong(0)
	jzentry := vars.GetLong(2)
	pos := vars.GetLong(4)
	byteArr := vars.GetRef(6)
	off := vars.GetInt(7)
	_len := vars.GetInt(8)

	goBytes := byteArr.GoBytes()
	goBytes = goBytes[off : off+_len]
	n := readEntry(jzentry, pos, goBytes)

	stack := frame.OperandStack()
	stack.PushInt(int32(n))
 */
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initIDs", "()V", TA(initIDs) },
        { "open", "(Ljava/lang/String;IJZ)J", TA(__open) },
        { "startsWithLOC", "(J)Z", TA(startsWithLOC) },
        { "getTotal", "(J)I", TA(getTotal) },
        { "getNextEntry", "(JI)J", TA(getNextEntry) },
        { "freeEntry", "(JJ)V", TA(freeEntry) },
        { "getEntry", "(J[BZ)J", TA(getEntry) },
        { "getEntryBytes", "(JI)[B", TA(getEntryBytes) },
        { "getEntryFlag", "(J)I", TA(getEntryFlag) },
        { "getEntryTime", "(J)J", TA(getEntryTime) },
        { "getEntryCrc", "(J)J", TA(getEntryCrc) },
        { "getEntrySize", "(J)J", TA(getEntrySize) },
        { "getEntryCSize", "(J)J", TA(getEntryCSize) },
        { "getEntryMethod", "(J)I", TA(getEntryMethod) },
        { "read", "((JJJ[BII)I", TA(__read) },
};

void java_util_zip_ZipFile_registerNatives()
{
    registerNatives("java/util/zip/ZipFile", methods, ARRAY_LENGTH(methods));
}
