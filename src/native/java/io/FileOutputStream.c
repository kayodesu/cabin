/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../interpreter/stack_frame.h"


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
 * @param append {@code true} to first advance the position to the
 *     end of file
 * @exception IOException If an I/O error has occurred.
 */
// private native void writeBytes(byte b[], int off, int len, boolean append) throws IOException;
static void writeBytes(struct stack_frame *frame)
{
//    auto thisObj = frame->getLocalVar(0).getRef();
//    auto b = frame->getLocalVar(1).getRef();
//    jint off = frame->getLocalVar(2).getInt();
//    jint len = frame->getLocalVar(3).getInt();
//    jbool append = jtypes::i2z(frame->getLocalVar(4).getInt());  // todo

    jvm_abort("error\n");
    // todo
}

// private native void close0() throws IOException;


void java_io_FileOutputStream_registerNatives()
{
    register_native_method("java/io/FileOutputStream", "registerNatives", "()V", empty_method);
    register_native_method("java/io/FileOutputStream", "writeBytes", "([BIIZ)V", writeBytes);
    register_native_method("java/io/FileOutputStream", "initIDs", "()V", initIDs);
}