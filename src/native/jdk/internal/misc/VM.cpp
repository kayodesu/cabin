/*
 * Author: Yo Ka
 */

#include <cassert>
#include "../../../../symbol.h"
#include "../../../jni_inner.h"

// private static native void initialize();
static void initialize()
{
    // todo
    Class *sys = loadBootClass(S(java_lang_System));
    initClass(sys);
    
    Method *m = sys->lookupStaticMethod("initPhase1", S(___V));
    assert(m != nullptr);
    execJavaFunc(m);
}

/*
 * Returns the first non-null class loader up the execution stack,
 * or null if only code from the null class loader is on the stack.
 *
 * public static native ClassLoader latestUserDefinedLoader();
 */
static jref latestUserDefinedLoader()
{
    jvm_abort("latestUserDefinedLoader"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initialize", "()V", (void *) initialize },
        { "latestUserDefinedLoader", "()Ljava/lang/ClassLoader;", (void *) latestUserDefinedLoader },
};

void jdk_internal_misc_VM_registerNatives()
{
    registerNatives("jdk/internal/misc/VM", methods, ARRAY_LENGTH(methods));
}
