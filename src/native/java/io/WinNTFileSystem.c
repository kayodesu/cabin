/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// private static native void initIDs();
static void initIDs(struct stack_frame *frame)
{
    // todo
}

void java_io_WinNTFileSystem_registerNatives()
{
    register_native_method("java/io/WinNTFileSystem~registerNatives~()V", registerNatives);
    register_native_method("java/io/WinNTFileSystem~initIDs~()V", initIDs);
}
