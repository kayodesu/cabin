#include "../../../../registry.h"
#include "../../../../../rtda/thread/Frame.h"

/*
 * Author: Jia Yang
 */

// private static native boolean VMSupportsCS8();
static void VMSupportsCS8(Frame *frame)
{
/*
 * 	stack := frame.OperandStack()
	stack.PushBoolean(false) // todo sync/atomic
 */
    frame->pushi(0); // todo
}

void java_util_concurrent_atomic_AtomicLong_registerNatives()
{
    register_native_method("java/util/concurrent/atomic/AtomicLong", "VMSupportsCS8", "()Z", VMSupportsCS8);
}
