#include "../../registry.h"
#include "../../../runtime/frame.h"

/*
 * Author: kayo
 */

static void setErrorMode(Frame *frame)
{
    frame->pushl(0); // todo
}

void sun_io_Win32ErrorMode_registerNatives()
{
    registerNative("sun/io/Win32ErrorMode", "setErrorMode", "(J)J", setErrorMode);
}
