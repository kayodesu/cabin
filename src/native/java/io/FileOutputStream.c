/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"
#include "../../../output.h"


// private static native void initIDs();
static void initIDs(struct stack_frame *frame)
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


/**
 * Writes the specified byte to this file output stream.
 *
 * @param   b   the byte to be written.
 * @param   append   {@code true} if the write operation first
 *     advances the position to the end of file
 *
 * private native void write(int b, boolean append) throws IOException;
 */

/**
 * Writes a sub array as a sequence of bytes.
 * @param b the data to be written
 * @param off the start offset in the data
 * @param len the number of bytes that are written
 * @param append {@code true} to first advance the position to the end of file
 * @exception IOException If an I/O error has occurred.
 */
// private native void writeBytes(byte b[], int off, int len, boolean append) throws IOException;
static void writeBytes(struct stack_frame *frame)
{
    jref this = slot_getr(frame->local_vars);
    jref b = slot_getr(frame->local_vars + 1);
    jint off = slot_geti(frame->local_vars + 2);
    jint len = slot_geti(frame->local_vars + 3);
    bool append = slot_geti(frame->local_vars + 4) == 0 ? false : true;

#ifdef JVM_DEBUG
    JOBJECT_CHECK_ARROBJ(b);
#endif
    // todo
    jbyte *data = jarrobj_data(b);
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


void java_io_FileOutputStream_registerNatives()
{
    register_native_method("java/io/FileOutputStream~registerNatives~()V", registerNatives);
    register_native_method("java/io/FileOutputStream~writeBytes~([BIIZ)V", writeBytes);
    register_native_method("java/io/FileOutputStream~initIDs~()V", initIDs);
}