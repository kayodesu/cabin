#include "../../../../jni_inner.h"
#include "../../../../../vmdef.h"

/*
 * Author: Yo Ka
 */

/*
 * Returns whether underlying JVM supports lockless CompareAndSet
 * for longs. Called only once and cached in VM_SUPPORTS_LONG_CAS.
 *
 * private static native boolean VMSupportsCS8();
 */
static jboolean VMSupportsCS8()
{
/*
 * 	stack := frame.OperandStack()
	stack.PushBoolean(false) // todo sync/atomic
 */
    return jfalse; // todo  sync/atomic
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "VMSupportsCS8", "()Z", (void *) VMSupportsCS8 },
};

void java_util_concurrent_atomic_AtomicLong_registerNatives()
{
    registerNatives("java/util/concurrent/atomic/AtomicLong", methods, ARRAY_LENGTH(methods));
}
