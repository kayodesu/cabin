/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"


/*
 * private static native Object newInstance0(Constructor<?> c, Object[] os)
 * throws InstantiationException, IllegalArgumentException, InvocationTargetException;
 */
static void newInstance0(struct stack_frame *frame)
{
    jref constructor_obj = slot_getr(frame->local_vars);
    jref arg_arr_obj = slot_getr(frame->local_vars + 1);

    jvm_abort("error\n");
}

void sun_reflect_NativeConstructorAccessorImpl_registerNatives()
{
    register_native_method("sun/reflect/NativeConstructorAccessorImpl", "registerNatives", "()V", empty_method);
    register_native_method("sun/reflect/NativeConstructorAccessorImpl", "newInstance0",
                           "(Ljava/lang/reflect/Constructor;[Ljava/lang/Object;)Ljava/lang/Object;", newInstance0);
}
