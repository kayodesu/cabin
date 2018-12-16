#include "../../registry.h"
#include "../../../jvm.h"

/*
 * Author: Jia Yang
 */

// private static native Object invoke0(Method method, Object o, Object[] os);
static void invoke0(struct stack_frame *frame)
{
    jvm_abort("error\n");
}

void sun_reflect_NativeMethodAccessorImpl_registerNatives()
{
    register_native_method("sun/reflect/NativeMethodAccessorImpl", "registerNatives", "()V", registerNatives);
    register_native_method("sun/reflect/NativeMethodAccessorImpl", "invoke0",
                           "(Ljava/lang/reflect/Method;Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;",
                           invoke0);
}
