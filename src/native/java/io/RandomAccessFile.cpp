#include "../../registry.h"
#include "../../../kayo.h"

/*
 * Author: kayo
 */

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
static void open0(Frame *frame)
{
    jvm_abort("open0");
}

// private native void close0() throws IOException;
static void close0(Frame *frame)
{
    jvm_abort("close0");
}

// private native void writeBytes(byte b[], int off, int len) throws IOException;
static void writeBytes(Frame *frame)
{
    jvm_abort("writeBytes");
}

// private native void write0(int b) throws IOException;
static void write0(Frame *frame)
{
    jvm_abort("write0");
}

// private native int readBytes(byte b[], int off, int len) throws IOException;
static void readBytes(Frame *frame)
{
    jvm_abort("readBytes");
}

// public native int read() throws IOException;
static void read(Frame *frame)
{
    jvm_abort("read");
}

// private native void seek0(long pos) throws IOException;
static void seek0(Frame *frame)
{
    jvm_abort("seek0");
}

// public native long getFilePointer() throws IOException;
static void getFilePointer(Frame *frame)
{
    jvm_abort("getFilePointer");
}

// public native long length() throws IOException;
static void length(Frame *frame)
{
    jvm_abort("length");
}

// public native void setLength(long newLength) throws IOException;
static void setLength(Frame *frame)
{
    jvm_abort("setLength");
}

void java_io_RandomAccessFile_registerNatives()
{
#undef C
#define C "java/io/RandomAccessFile"
    registerNative(C, "open0", "(Ljava/lang/String;I)V", open0);
    registerNative(C, "close0", "()V", close0);
    registerNative(C, "writeBytes", "([BII)V", writeBytes);
    registerNative(C, "write0", "(I)V", write0);
    registerNative(C, "readBytes", "([BII)I", readBytes);
    registerNative(C, "read", "()I", read);
    registerNative(C, "seek0", "(J)V", seek0);
    registerNative(C, "getFilePointer", "()J", getFilePointer);
    registerNative(C, "length", "()J", length);
    registerNative(C, "setLength", "(J)V", setLength);
}
