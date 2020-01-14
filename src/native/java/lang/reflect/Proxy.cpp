#include "../../../registry.h"
#include "../../../../kayo.h"

/*
 * Author: kayo
 */

//private static native Class<?> defineClass0(ClassLoader loader, String name, byte[] b, int off, int len);
static void defineClass0(Frame *frame)
{
    jvm_abort("defineClass0");
}

void java_lang_reflect_Proxy_registerNatives()
{
    registerNative("java/lang/reflect/Proxy", "defineClass0",
                   "(Ljava/lang/ClassLoader;Ljava/lang/String;[BII)Ljava/lang/Class;", defineClass0);
}