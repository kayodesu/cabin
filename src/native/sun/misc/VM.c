/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../interpreter/interpreter.h"

// private static native void initialize();
static void initialize(struct frame *frame)
{
//    static bool flag = false;
//    if (flag) {
//        return;
//    }

    // todo
    struct class *sys_class = load_sys_class("java/lang/System");
//    sysClass->clinit(frame);  // todo
    struct method *m = class_lookup_static_method(sys_class, "initializeSystemClass", "()V");
    if (m == NULL) {
        jvm_abort("not find initializeSystemClass\n");  // todo m == nullptr
    }

    exec_java_func(m, NULL);
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
