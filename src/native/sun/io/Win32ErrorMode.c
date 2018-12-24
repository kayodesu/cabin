#include "../../registry.h"
#include "../../../rtda/thread/frame.h"

/*
 * Author: Jia Yang
 */

static void setErrorMode(struct frame *frame)
{
    frame_stack_pushl(frame, 0); // todo
}

void sun_io_Win32ErrorMode_registerNatives()
{
    register_native_method("sun/io/Win32ErrorMode~registerNatives~()V", registerNatives);
    register_native_method("sun/io/Win32ErrorMode~setErrorMode~(J)J", setErrorMode);
}
