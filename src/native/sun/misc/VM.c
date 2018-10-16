/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// private static native void initialize();
static void initialize(struct stack_frame *frame)
{
    // todo
    struct jclass *sys_class = classloader_load_class(frame->method->jclass->loader, "java/lang/System");
//    sysClass->clinit(frame);
    struct jmethod *m = jclass_lookup_static_method(sys_class, "initializeSystemClass", "()V");
    if (m == NULL) {
        jvm_abort("not find initializeSystemClass\n");
    }
    // todo m == nullptr
    sf_invoke_method(frame, m, NULL); // frame->invokeMethod(m);
}

void sun_misc_VM_registerNatives(struct stack_frame *frame)
{
    register_native_method("sun/misc/VM", "initialize", "()V", initialize);
}
