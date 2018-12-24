/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static void defineClass1(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private native Class<?> findBootstrapClass(String name);
static void findBootstrapClass(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private native final Class<?> findLoadedClass0(String name);
static void findLoadedClass0(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private static native String findBuiltinLib(String name);
static void findBuiltinLib(struct frame *frame)
{
//    jvm_abort("findBuiltinLib");

    // todo
    jref name0 = frame_locals_getr(frame, 0);

    const char *name = jstrobj_value(name0);
//    printvm("findBuiltinLib, %s\n", name);
    if (strcmp(name, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
        char buf[1024] = "C:\\Program Files\\Java\\jre1.8.0_162\\bin\\zip.dll";  // todo
        frame_stack_pushr(frame, jstrobj_create(buf));    // todo
    } else {
        jvm_abort(name);
    }
//    os_pushr(frame->operand_stack, name0);  // todo
}

void java_lang_ClassLoader_registerNatives()
{
    register_native_method("java/lang/ClassLoader~registerNatives~()V", registerNatives);
    register_native_method("java/lang/ClassLoader~defineClass1~"
             "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                           defineClass1);
    register_native_method("java/lang/ClassLoader~findBootstrapClass~(Ljava/lang/String;)Ljava/lang/Class;", findBootstrapClass);
    register_native_method("java/lang/ClassLoader~findLoadedClass0~(Ljava/lang/String;)Ljava/lang/Class;", findLoadedClass0);
    register_native_method("java/lang/ClassLoader~findBuiltinLib~(Ljava/lang/String;)Ljava/lang/String;", findBuiltinLib);
}

