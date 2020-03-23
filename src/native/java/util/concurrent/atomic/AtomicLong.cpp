#include "../../../../registry.h"
#include "../../../../../runtime/frame.h"

/*
 * Author: kayo
 */

/*
 * Returns whether underlying JVM supports lockless CompareAndSet
 * for longs. Called only once and cached in VM_SUPPORTS_LONG_CAS.
 *
 * private static native boolean VMSupportsCS8();
 */
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
    registerNative("java/util/concurrent/atomic/AtomicLong", "VMSupportsCS8", "()Z", VMSupportsCS8);
}
