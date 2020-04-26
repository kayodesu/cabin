/*
 * Author: Yo Ka
 */

#include "../../../interpreter/interpreter.h"
#include "../../../symbol.h"
#include "../../../objects/class.h"
#include "../../../objects/class_loader.h"
#include "../../jni_inner.h"
#include "../../../runtime/frame.h"

// private static native void initialize();
static void initialize(JNIEnv *env, jclass clazz)
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
static jref latestUserDefinedLoader(JNIEnv *env, jclass clazz)
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
