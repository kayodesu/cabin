/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static void defineClass1(struct stack_frame *frame)
{
    // todo
    jvm_abort("");
}

// private native Class<?> findBootstrapClass(String name);
static void findBootstrapClass(struct stack_frame *frame)
{
    // todo
    jvm_abort("");
}

// private native final Class<?> findLoadedClass0(String name);
static void findLoadedClass0(struct stack_frame *frame)
{
    // todo
    jvm_abort("");
}

void java_lang_ClassLoader_registerNatives()
{
#define R(method, descriptor) register_native_method("java/lang/ClassLoader", #method, descriptor, method)

    R(registerNatives, "()V");
    R(defineClass1, "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;");
    R(findBootstrapClass, "(Ljava/lang/String;)Ljava/lang/Class;");
    R(findLoadedClass0, "(Ljava/lang/String;)Ljava/lang/Class;");

#undef R
}

