#include "../../../jni_internal.h"

//private static native Class<?> defineClass0(ClassLoader loader, String name, byte[] b, int off, int len);
static jclass defineClass0(jobject loader, jstring name, jbyteArray b, jint off, jint len)
{
    jvm_abort("defineClass0");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "defineClass0", "(Ljava/lang/ClassLoader;" STR "[BII)" CLS, (void *) defineClass0 },
};

void java_lang_reflect_Proxy_registerNatives()
{
    registerNatives("java/lang/reflect/Proxy", methods, ARRAY_LENGTH(methods));
}