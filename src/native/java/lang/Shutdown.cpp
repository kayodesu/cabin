#include "../../jni_internal.h"

/*
 * Notify the VM that it's time to halt.
 *
 * static native void beforeHalt();
 */
static void beforeHalt()
{
    jvm_abort("beforeHalt");
}

/*
 * The halt method is synchronized on the halt lock
 * to avoid corruption of the delete-on-shutdown file list.
 * It invokes the true native halt method.
 *
 * static native void halt0(int status);
 */
static void halt0(jint status)
{
    jvm_abort("halt0");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "beforeHalt", "()V", (void *) beforeHalt },
        { "halt0", "(I)V", (void *) halt0 },
};

void java_lang_Shutdown_registerNatives()
{
    registerNatives("java/lang/Shutdown", methods, ARRAY_LENGTH(methods));
}
