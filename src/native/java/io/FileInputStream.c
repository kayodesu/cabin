/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"

// private native void initIDs();
static void initIDs(struct frame *frame)
{
//    jvm_abort("error\n");
    // todo
}

/**
 * Opens the specified file for reading.
 * @param name the name of the file
 *
 * private native void open0(String name) throws FileNotFoundException;
 */

/**
 * Reads a byte of data from this input stream. This method blocks
 * if no input is yet available.
 *
 * @return     the next byte of data, or <code>-1</code> if the end of the
 *             file is reached.
 * @exception  IOException  if an I/O error occurs.
 *
 * private native int read0() throws IOException;
 */


/**
 * Reads a subarray as a sequence of bytes.
 * @param b the data to be written
 * @param off the start offset in the data
 * @param len the number of bytes that are written
 * @exception IOException If an I/O error has occurred.
 *
 * private native int readBytes(byte b[], int off, int len) throws IOException;
 */


/**
 * Skips over and discards <code>n</code> bytes of data from the
 * input stream.
 *
 * <p>The <code>skip</code> method may, for a variety of
 * reasons, end up skipping over some smaller number of bytes,
 * possibly <code>0</code>. If <code>n</code> is negative, the method
 * will try to skip backwards. In case the backing file does not support
 * backward skip at its current position, an <code>IOException</code> is
 * thrown. The actual number of bytes skipped is returned. If it skips
 * forwards, it returns a positive value. If it skips backwards, it
 * returns a negative value.
 *
 * <p>This method may skip more bytes than what are remaining in the
 * backing file. This produces no exception and the number of bytes skipped
 * may include some number of bytes that were beyond the EOF of the
 * backing file. Attempting to read from the stream after skipping past
 * the end will result in -1 indicating the end of the file.
 *
 * @param      n   the number of bytes to be skipped.
 * @return     the actual number of bytes skipped.
 * @exception  IOException  if n is negative, if the stream does not
 *             support seek, or if an I/O error occurs.
 *
 * public native long skip(long n) throws IOException;
 */

/**
 * Returns an estimate of the number of remaining bytes that can be read (or
 * skipped over) from this input stream without blocking by the next
 * invocation of a method for this input stream. Returns 0 when the file
 * position is beyond EOF. The next invocation might be the same thread
 * or another thread. A single read or skip of this many bytes will not
 * block, but may read or skip fewer bytes.
 *
 * <p> In some cases, a non-blocking read (or skip) may appear to be
 * blocked when it is merely slow, for example when reading large
 * files over slow networks.
 *
 * @return     an estimate of the number of remaining bytes that can be read
 *             (or skipped over) from this input stream without blocking.
 * @exception  IOException  if this file input stream has been closed by calling
 *             {@code close} or an I/O error occurs.
 *
 * public native int available() throws IOException;
 */

/*
 * private native void close0() throws IOException;
 */

void java_io_FileInputStream_registerNatives()
{
    register_native_method("java/io/FileInputStream~registerNatives~()V", registerNatives);
    register_native_method("java/io/FileInputStream~initIDs~()V", initIDs);
}