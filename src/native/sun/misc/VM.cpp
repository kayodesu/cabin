/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"
#include "../../../objects/Class.h"
#include "../../../objects/class_loader.h"

// private static native void initialize();
static void initialize(Frame *frame)
{
    // todo
    Class *sys = loadBootClass(S(java_lang_System));
    initClass(sys);
//    sys->clinit();
    Method *m = sys->lookupStaticMethod("initializeSystemClass", S(___V));
    assert(m != nullptr);
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
    registerNative("sun/misc/VM", "initialize", "()V", initialize);
    registerNative("sun/misc/VM", "latestUserDefinedLoader", "()Ljava/lang/ClassLoader;", latestUserDefinedLoader);
}
