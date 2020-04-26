/*
 * Author: Yo Ka
 */

#include <assert.h>
#include "../../../symbol.h"
#include "../../jnidef.h"

// private static native void initialize();
static void initialize(JNIEnv *env, jclass clazz)
{
    // todo
    jclass sys = (*env)->FindClass(env, S(java_lang_System));
    ci_initClass(sys);

    jmethodID m = (*env)->GetStaticMethodID(env, sys, "initializeSystemClass", S(___V));
    assert(m != NULL);
    (*env)->CallStaticVoidMethod(env, sys, m);
}

/*
 * Returns the first non-null class loader up the execution stack,
 * or null if only code from the null class loader is on the stack.
 *
 * public static native ClassLoader latestUserDefinedLoader();
 */
static jobject latestUserDefinedLoader(JNIEnv *env, jclass clazz)
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
