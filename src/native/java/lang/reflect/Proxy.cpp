#include "../../../jni_inner.h"
#include "../../../../vmdef.h"
/*
 * Author: Yo Ka
 */

//private static native Class<?> defineClass0(ClassLoader loader, String name, byte[] b, int off, int len);
static jclsref defineClass0(jref loader, jstrref name, jarrref b, jint off, jint len)
{
    jvm_abort("defineClass0");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "defineClass0", "(Ljava/lang/ClassLoader;Ljava/lang/String;[BII)" CLS, (void *) defineClass0 },
};

void java_lang_reflect_Proxy_registerNatives()
{
    registerNatives("java/lang/reflect/Proxy", methods, ARRAY_LENGTH(methods));
}