/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../../symbol.h"
#include "helper.h"


// private static native void initIDs();
static void initIDs(JNIEnv *env, jobject _this)
{
//    jvm_abort("error\n");
    // todo
}

/*
 * Opens a file, with the specified name, for overwriting or appending.
 *
 * private native void open0(String name, boolean append) throws FileNotFoundException;
 */
static void open0(JNIEnv *env, jobject _this, jstring name, jboolean append)
{
    if (append)
        __openFile(env, _this, name, "ab");
    else
        __openFile(env, _this, name, "wb");
}

/**
 * Writes the specified byte to this file output stream.
 *
 * @param   b   the byte to be written.
 * @param   append   {@code true} if the write operation first
 *     advances the position to the end of file
 *
 * private native void write(int b, boolean append) throws IOException;
 */
static void write(JNIEnv *env, jobject _this, jint b, jboolean append)
{
    jvm_abort("error\n"); // todo
}


/**
 * Writes a sub array as a sequence of bytes.
 * @param b the data to be written
 * @param off the start offset in the data
 * @param len the number of bytes that are written
 * @param append {@code true} to first advance the position to the end of file
 * @exception IOException If an I/O error has occurred.
 */
// private native void writeBytes(byte b[], int off, int len, boolean append) throws IOException;
static void writeBytes(JNIEnv *env, jobject _this, jbyteArray b, jint off, jint len, jboolean append)
{
    jbyte *data = (*env)->GetByteArrayElements(env, b, NULL);

    // todo 这里默认输出到了控制台，是不对的
    // todo 应该根据_this来选择输出位置
    char *chars = (char *) (data + off);
    for (jint i = 0; i < len; i++) {
        printf("%c", chars[i]);
    }
    fflush(stdout);  // todo

    /*
    fdObj := fosObj.GetFieldValue("fd~Ljava/io/FileDescriptor;").(*heap.Object)
	if fdObj.Extra() == nil {
		goFd := fdObj.GetFieldValue("fd", "I").(int32)
		switch goFd {
		case 0:
			fdObj.SetExtra(os.Stdin)
		case 1:
			fdObj.SetExtra(os.Stdout)
		case 2:
			fdObj.SetExtra(os.Stderr)
		}
	}
	goFile := fdObj.Extra().(*os.File)

	goBytes := byteArrObj.GoBytes()
	goBytes = goBytes[offset : offset+length]
	goFile.Write(goBytes)
     */
}

// private native void close0() throws IOException;
static void close0(JNIEnv *env, jobject _this)
{
    __closeFile(env, _this);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "writeBytes", "([BIIZ)V", (void *) writeBytes },
        { "initIDs", "()V", (void *) initIDs },
        { "open0", "(Ljava/lang/String;Z)V", (void *) open0 },
        { "write", "(IZ)V", (void *) write },
        { "close0", "()V", (void *) close0 },
};

void java_io_FileOutputStream_registerNatives()
{
    registerNatives("java/io/FileOutputStream", methods, ARRAY_LENGTH(methods));
}