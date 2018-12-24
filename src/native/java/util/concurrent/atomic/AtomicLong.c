#include "../../../../registry.h"
#include "../../../../../rtda/thread/frame.h"

/*
 * Author: Jia Yang
 */

// private static native boolean VMSupportsCS8();
static void VMSupportsCS8(struct frame *frame)
{
/*
 * 	stack := frame.OperandStack()
	stack.PushBoolean(false) // todo sync/atomic
 */
    frame_stack_pushi(frame, 0); // todo
}

void java_util_concurrent_atomic_AtomicLong_registerNatives()
{
    register_native_method("java/util/concurrent/atomic/AtomicLong~registerNatives~()V", registerNatives);
    register_native_method("java/util/concurrent/atomic/AtomicLong~VMSupportsCS8~()Z", VMSupportsCS8);
}
