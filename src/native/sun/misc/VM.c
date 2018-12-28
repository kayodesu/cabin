/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"

// private static native void initialize();
static void initialize(struct frame *frame)
{
//    static bool flag = false;
//    if (flag) {
//        return;
//    }

    // todo
    struct class *sys_class = classloader_load_class(frame->m.method->clazz->loader, "java/lang/System");
//    sysClass->clinit(frame);  // todo
    struct method *m = class_lookup_static_method(sys_class, "initializeSystemClass", "()V");
    if (m == NULL) {
        jvm_abort("not find initializeSystemClass\n");  // todo m == nullptr
    }

    thread_invoke_method(frame->thread, m, NULL);
}

/*
 * Returns the first non-null class loader up the execution stack,
 * or null if only code from the null class loader is on the stack.
 *
 * public static native ClassLoader latestUserDefinedLoader();
 */
static void latestUserDefinedLoader(struct frame *frame)
{
    jvm_abort(""); // todo
}

void sun_misc_VM_registerNatives()
{
    register_native_method("sun/misc/VM~registerNatives~()V", registerNatives);
    register_native_method("sun/misc/VM~initialize~()V", initialize);
    register_native_method("sun/misc/VM~latestUserDefinedLoader~()Ljava/lang/ClassLoader;", latestUserDefinedLoader);
}
