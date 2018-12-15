/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"

// private static native void initialize();
static void initialize(struct stack_frame *frame)
{
//    static bool flag = false;
//    if (flag) {
//        return;
//    }

    // todo
    struct jclass *sys_class = classloader_load_class(frame->method->jclass->loader, "java/lang/System");
//    sysClass->clinit(frame);  // todo
    struct jmethod *m = jclass_lookup_static_method(sys_class, "initializeSystemClass", "()V");
    if (m == NULL) {
        jvm_abort("not find initializeSystemClass\n");  // todo m == nullptr
    }

    jthread_invoke_method(frame->thread, m, NULL);
}

void sun_misc_VM_registerNatives()
{
    register_native_method("sun/misc/VM", "registerNatives", "()V", empty_method);
    register_native_method("sun/misc/VM", "initialize", "()V", initialize);
}
