#include "../../jni_internal.h"

/*
 * private static final int O_RDONLY = 1;
    private static final int O_RDWR =   2;
    private static final int O_SYNC =   4;
    private static final int O_DSYNC =  8;
 */

/*
 * Opens a file and returns the file type. The file is
 * opened in read-write mode if the O_RDWR bit in mode
 * is true, else the file is opened as read-only.
 * If the name refers to a directory, an IOException
 * is thrown.
 *
 * private native void open0(String name, int mode) throws FileNotFoundException;
 */
static void open0(jobject _this, jstring name, jint mode)
{
    JVM_PANIC("open0");
}

// private native void close0() throws IOException;
static void close0(jobject _this)
{
    JVM_PANIC("close0");
}

// private native void writeBytes(byte b[], int off, int len) throws IOException;
static void writeBytes(jobject _this, jobject b, jint off, jint len)
{
    JVM_PANIC("writeBytes");
}

// private native void write0(int b) throws IOException;
static void write0(jobject _this, jint b)
{
    JVM_PANIC("write0");
}

// private native int readBytes(byte b[], int off, int len) throws IOException;
static void readBytes(jobject _this, jobject b, jint off, jint len)
{
    JVM_PANIC("readBytes");
}

// public native int read() throws IOException;
static void read(jobject _this)
{
    JVM_PANIC("read");
}

// private native void seek0(long pos) throws IOException;
static void seek0(jobject _this, jlong pos)
{
    JVM_PANIC("seek0");
}

// public native long getFilePointer() throws IOException;
static jlong getFilePointer(jobject _this)
{
    JVM_PANIC("getFilePointer");
}

// public native long length() throws IOException;
static jlong length(jobject _this)
{
    JVM_PANIC("length");
}

// public native void setLength(long newLength) throws IOException;
static void setLength(jobject _this, jlong newLength)
{
    JVM_PANIC("setLength");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "open0", "(Ljava/lang/String;I)V", TA(open0) },
        { "close0", "()V", TA(close0) },
        { "writeBytes", "([BII)V", TA(writeBytes) },
        { "write0", "(I)V", TA(write0) },
        { "readBytes", "([BII)I", TA(readBytes) },
        { "read", "()I", TA(read) },
        { "seek0", "(J)V", TA(seek0) },
        { "getFilePointer", "()J", TA(getFilePointer) },
        { "length", "()J", TA(length) },
        { "setLength", "(J)V", TA(setLength) },
};

void java_io_RandomAccessFile_registerNatives()
{
    registerNatives("java/io/RandomAccessFile", methods, ARRAY_LENGTH(methods));
}
