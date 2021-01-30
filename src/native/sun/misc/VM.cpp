#include <cassert>
#include "../../../symbol.h"
#include "../../jni_inner.h"
#include "../../../objects/class_loader.h"
#include "../../../metadata/method.h"
#include "../../../metadata/class.h"
#include "../../../interpreter/interpreter.h"

// private static native void initialize();
static void initialize()
{
    // todo
    Class *sys = loadBootClass(S(java_lang_System));
    initClass(sys);

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
static jobject latestUserDefinedLoader()
{
    jvm_abort("latestUserDefinedLoader"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initialize", "()V", (void *) initialize },
        { "latestUserDefinedLoader", "()Ljava/lang/ClassLoader;", (void *) latestUserDefinedLoader },
};

void sun_misc_VM_registerNatives()
{
    registerNatives("sun/misc/VM", methods, ARRAY_LENGTH(methods));
}
