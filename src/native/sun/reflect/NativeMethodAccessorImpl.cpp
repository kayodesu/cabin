#include "../../jni_inner.h"
#include "../../../jvmstd.h"

/*
 * Author: Yo Ka
 */

// private static native Object invoke0(Method method, Object o, Object[] os);
static void invoke0(jref method, jref o, jarrref os)
{
    jvm_abort("error\n");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "invoke0", "(Ljava/lang/reflect/Method;" OBJ "[Ljava/lang/Object;)" OBJ, (void *) invoke0 },
};

void sun_reflect_NativeMethodAccessorImpl_registerNatives()
{
    registerNatives("sun/reflect/NativeMethodAccessorImpl", methods, ARRAY_LENGTH(methods));
}
