/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"

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

// private static native String findBuiltinLib(String name);
static void findBuiltinLib(struct stack_frame *frame)
{
//    jvm_abort("findBuiltinLib");

    // todo
    jref name0 = slot_getr(frame->local_vars);

#ifdef JVM_DEBUG
    JOBJECT_CHECK_STROBJ(name0);
#endif
    const char *name = jstrobj_value(name0);
//    printvm("findBuiltinLib, %s\n", name);
    if (strcmp(name, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
        char buf[1024] = "C:\\Program Files\\Java\\jre1.8.0_162\\bin\\zip.dll";  // todo
        os_pushr(frame->operand_stack, jstrobj_create(buf));    // todo
    } else {
        jvm_abort(name);
    }
//    os_pushr(frame->operand_stack, name0);  // todo
}

void java_lang_ClassLoader_registerNatives()
{
#define R(method, descriptor) register_native_method("java/lang/ClassLoader", #method, descriptor, method)

    R(registerNatives, "()V");
    R(defineClass1, "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;");
    R(findBootstrapClass, "(Ljava/lang/String;)Ljava/lang/Class;");
    R(findLoadedClass0, "(Ljava/lang/String;)Ljava/lang/Class;");
    R(findBuiltinLib, "(Ljava/lang/String;)Ljava/lang/String;");

#undef R
}

