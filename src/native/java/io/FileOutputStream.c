/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/object.h"
#include "../../../output.h"
#include "../../../rtda/heap/arrobj.h"


// private static native void initIDs();
static void initIDs(struct frame *frame)
{
//    jvm_abort("error\n");
    // todo
}

/**
 * Opens a file, with the specified name, for overwriting or appending.
 * @param name name of file to be opened
 * @param append whether the file is to be opened in append mode
 *
 * private native void open0(String name, boolean append) throws FileNotFoundException;
 */
static void open0(struct frame *frame)
{
    jvm_abort("error\n"); // todo
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
static void write(struct frame *frame)
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
static void writeBytes(struct frame *frame)
{
    jref this = frame_locals_getr(frame, 0);
    jref b = frame_locals_getr(frame, 1);
    jint off = frame_locals_geti(frame, 2);
    jint len = frame_locals_geti(frame, 3);
    bool append = frame_locals_getz(frame, 4);

    // todo
    jbyte *data = arrobj_data(b);
    write_bytes(this, data + off, len, append);
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
static void close0(struct frame *frame)
{
    jvm_abort("error\n"); // todo
}


void java_io_FileOutputStream_registerNatives()
{
    register_native_method("java/io/FileOutputStream~registerNatives~()V", registerNatives);
    register_native_method("java/io/FileOutputStream~writeBytes~([BIIZ)V", writeBytes);
    register_native_method("java/io/FileOutputStream~initIDs~()V", initIDs);
    register_native_method("java/io/FileOutputStream~open0~(Ljava/lang/String;Z)V", open0);
    register_native_method("java/io/FileOutputStream~write~(IZ)V", write);
    register_native_method("java/io/FileOutputStream~close0~()V", close0);
}