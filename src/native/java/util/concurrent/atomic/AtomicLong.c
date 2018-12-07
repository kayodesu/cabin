#include "../../../../registry.h"
#include "../../../../../interpreter/operand_stack.h"
#include "../../../../../interpreter/stack_frame.h"

/*
 * Author: Jia Yang
 */

// private static native boolean VMSupportsCS8();
static void VMSupportsCS8(struct stack_frame *frame)
{
/*
 * 	stack := frame.OperandStack()
	stack.PushBoolean(false) // todo sync/atomic
 */
    os_pushi(frame->operand_stack, 0); // todo
}

void java_util_concurrent_atomic_AtomicLong_registerNatives()
{
    register_native_method("java/util/concurrent/atomic/AtomicLong", "registerNatives", "()V", empty_method);
    register_native_method("java/util/concurrent/atomic/AtomicLong", "VMSupportsCS8", "()Z", VMSupportsCS8);
}
