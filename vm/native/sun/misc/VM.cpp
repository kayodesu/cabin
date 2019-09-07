/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"
#include "../../../rtda/ma/Class.h"

// private static native void initialize();
static void initialize(Frame *frame)
{
//    static bool flag = false;
//    if (flag) {
//        return;
//    }

    // todo
    Class *sys_class = loadSysClass(S(java_lang_System));
//    sysClass->clinit(frame);  // todo
    Method *m = sys_class->lookupStaticMethod("initializeSystemClass", S(___V));
    if (m == nullptr) {
        jvm_abort("not find initializeSystemClass\n");  // todo m == nullptr
    }

    execJavaFunc(m);
}

/*
 * Returns the first non-null class loader up the execution stack,
 * or null if only code from the null class loader is on the stack.
 *
 * public static native ClassLoader latestUserDefinedLoader();
 */
static void latestUserDefinedLoader(Frame *frame)
{
    jvm_abort("latestUserDefinedLoader"); // todo
}

void sun_misc_VM_registerNatives()
{
    register_native_method("sun/misc/VM", "initialize", "()V", initialize);
    register_native_method("sun/misc/VM", "latestUserDefinedLoader", "()Ljava/lang/ClassLoader;", latestUserDefinedLoader);
}
