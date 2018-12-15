#include "../../registry.h"
#include "../../../interpreter/operand_stack.h"
#include "../../../interpreter/stack_frame.h"

/*
 * Author: Jia Yang
 */

static void setErrorMode(struct stack_frame *frame)
{
    os_pushl(frame->operand_stack, 0); // todo
}

void sun_io_Win32ErrorMode_registerNatives()
{
    register_native_method("sun/io/Win32ErrorMode", "setErrorMode", "(J)J", setErrorMode);
}
