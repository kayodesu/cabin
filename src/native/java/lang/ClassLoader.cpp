/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../kayo.h"
#include "../../../rtda/thread/Frame.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/heap/StringObject.h"

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static void defineClass0(Frame *frame)
{
    // todo
    jvm_abort("defineClass0");
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static void defineClass1(Frame *frame)
{
    // todo
    jvm_abort("defineClass1");
}

// private native Class<?> defineClass2(String name, java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
static void defineClass2(Frame *frame)
{
    // todo
    jvm_abort("defineClass2");
}

// private native void resolveClass0(Class<?> c);
static void resolveClass0(Frame *frame)
{
    // todo
    jvm_abort("resolveClass0");
}

// private native Class<?> findBootstrapClass(String name);
static void findBootstrapClass(Frame *frame)
{
    // todo
    jvm_abort("findBootstrapClass");
}

// private native final Class<?> findLoadedClass0(String name);
static void findLoadedClass0(Frame *frame)
{
    // todo
    jvm_abort("findLoadedClass0");
}

// private static native String findBuiltinLib(String name);
static void findBuiltinLib(Frame *frame)
{
    // todo
    auto name0 = frame->getLocalAsRef<StringObject>(0);

    const char *name = name0->getUtf8Value();
    if (strcmp(name, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = R"(C:\Progles (x86)\Java\jre1.8.0_221\bin\zip.dll)";  // todo
        frame->pushr(StringObject::newInst(buf));    // todo
    } else {
        jvm_abort(name);
    }
//    os_pushr(frame->operand_stack, name0);  // todo
}
// Retrieves the assertion directives from the VM.
// private static native AssertionStatusDirectives retrieveDirectives();
static void retrieveDirectives(Frame *frame)
{
    // todo
    jvm_abort("retrieveDirectives");
}

void java_lang_ClassLoader_registerNatives()
{
#undef C
#define C "java/lang/ClassLoader",
    register_native_method(C"defineClass0",
             "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;",
                           defineClass0);
    register_native_method(C"defineClass1",
             "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                           defineClass1);
    register_native_method(C"defineClass2",
             "(Ljava/lang/String;Ljava/nio/ByteBuffer;IILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                           defineClass2);
    register_native_method(C"resolveClass0", "(Ljava/lang/Class;)V", resolveClass0);
    register_native_method(C"findBootstrapClass", "(Ljava/lang/String;)Ljava/lang/Class;", findBootstrapClass);
    register_native_method(C"findLoadedClass0", "(Ljava/lang/String;)Ljava/lang/Class;", findLoadedClass0);
    register_native_method(C"findBuiltinLib", "(Ljava/lang/String;)Ljava/lang/String;", findBuiltinLib);
    register_native_method(C"retrieveDirectives", "()Ljava/lang/AssertionStatusDirectives;", retrieveDirectives);
}

