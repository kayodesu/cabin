/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"

/*
 * This routine initializes JNI field offsets for the class
 *
 * private native void initIDs();
 */
static void initIDs(struct frame *frame)
{
    // todo
//    jvm_abort("error\n");
}

/*
 * private static native long set(int d);
 */
static void set(struct frame *frame)
{
//    jvm_abort("error\n");
    // todo
    frame_stack_pushl(frame, 123);
}

/**
 * Force all system buffers to synchronize with the underlying
 * device.  This method returns after all modified data and
 * attributes of this FileDescriptor have been written to the
 * relevant device(s).  In particular, if this FileDescriptor
 * refers to a physical storage medium, such as a file in a file
 * system, sync will not return until all in-memory modified copies
 * of buffers associated with this FileDesecriptor have been
 * written to the physical medium.
 *
 * sync is meant to be used by code that requires physical
 * storage (such as a file) to be in a known state  For
 * example, a class that provided a simple transaction facility
 * might use sync to ensure that all changes to a file caused
 * by a given transaction were recorded on a storage medium.
 *
 * sync only affects buffers downstream of this FileDescriptor.  If
 * any in-memory buffering is being done by the application (for
 * example, by a BufferedOutputStream object), those buffers must
 * be flushed into the FileDescriptor (for example, by invoking
 * OutputStream.flush) before that data will be affected by sync.
 *
 * @exception SyncFailedException
 *        Thrown when the buffers cannot be flushed,
 *        or because the system cannot guarantee that all the
 *        buffers have been synchronized with physical media.
 * @since     JDK1.1
 *
 * public native void sync() throws SyncFailedException;
 */
static void sync(struct frame *frame)
{
    jvm_abort("error\n"); // todo
}

void java_io_FileDescriptor_registerNatives()
{
    register_native_method("java/io/FileDescriptor~registerNatives~()V", registerNatives);
    register_native_method("java/io/FileDescriptor~initIDs~()V", initIDs);
    register_native_method("java/io/FileDescriptor~set~(I)J", set);
    register_native_method("java/io/FileDescriptor~sync~()V", sync);
}