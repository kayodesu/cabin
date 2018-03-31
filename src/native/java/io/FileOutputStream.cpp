/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/objectarea/JarrayObj.h"


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
 * @param append {@code true} to first advance the position to the
 *     end of file
 * @exception IOException If an I/O error has occurred.
 */
// private native void writeBytes(byte b[], int off, int len, boolean append) throws IOException;
static void writeBytes(StackFrame *frame) {
    auto thisObj = static_cast<Jobject *>(frame->getLocalVars(0).getReference());
    auto b = static_cast<JarrayObj *>(frame->getLocalVars(1).getReference());
    jint off = frame->getLocalVars(2).getInt();
    jint len = frame->getLocalVars(3).getInt();
    jbool append = frame->getLocalVars(4).getInt();  // todo

    jvmAbort("error\n");
    // todo
}

// private native void close0() throws IOException;

// private static native void initIDs();
static void initIDs(StackFrame *frame) {
//    jvmAbort("error\n");
    // todo
}

void java_io_FileOutputStream_registerNatives() {
    registerNativeMethod("java/io/FileOutputStream", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/io/FileOutputStream", "writeBytes", "([BIIZ)V", writeBytes);
    registerNativeMethod("java/io/FileOutputStream", "initIDs", "()V", initIDs);
}