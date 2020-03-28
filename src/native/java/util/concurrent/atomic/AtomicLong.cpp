#include "../../../../jni_inner.h"
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

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "VMSupportsCS8", "()Z", (void *) VMSupportsCS8 },
};

void java_util_concurrent_atomic_AtomicLong_registerNatives()
{
    registerNatives("java/util/concurrent/atomic/AtomicLong", methods, ARRAY_LENGTH(methods));
}
