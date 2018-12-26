/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static void defineClass0(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static void defineClass1(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private native Class<?> defineClass2(String name, java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
static void defineClass2(struct frame *frame)
{
    // todo
    jvm_abort("");
}

// private native void resolveClass0(Class<?> c);
static void resolveClass0(struct frame *frame)
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
        frame_stack_pushr(frame, strobj_create(buf));    // todo
    } else {
        jvm_abort(name);
    }
//    os_pushr(frame->operand_stack, name0);  // todo
}
// Retrieves the assertion directives from the VM.
// private static native AssertionStatusDirectives retrieveDirectives();
static void retrieveDirectives(struct frame *frame)
{
    // todo
    jvm_abort("");
}

void java_lang_ClassLoader_registerNatives()
{
    register_native_method("java/lang/ClassLoader~registerNatives~()V", registerNatives);
    register_native_method("java/lang/ClassLoader~defineClass0~"
             "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;",
                           defineClass0);
    register_native_method("java/lang/ClassLoader~defineClass1~"
             "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                           defineClass1);
    register_native_method("java/lang/ClassLoader~defineClass2~"
             "(Ljava/lang/String;Ljava/nio/ByteBuffer;IILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                           defineClass2);
    register_native_method("java/lang/ClassLoader~resolveClass0~(Ljava/lang/Class;)V", resolveClass0);
    register_native_method("java/lang/ClassLoader~findBootstrapClass~(Ljava/lang/String;)Ljava/lang/Class;", findBootstrapClass);
    register_native_method("java/lang/ClassLoader~findLoadedClass0~(Ljava/lang/String;)Ljava/lang/Class;", findLoadedClass0);
    register_native_method("java/lang/ClassLoader~findBuiltinLib~(Ljava/lang/String;)Ljava/lang/String;", findBuiltinLib);
    register_native_method("java/lang/ClassLoader~retrieveDirectives~()Ljava/lang/AssertionStatusDirectives;", retrieveDirectives);
}

