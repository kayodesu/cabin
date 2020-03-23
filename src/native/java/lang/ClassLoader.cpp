/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../kayo.h"
#include "../../../objects/class.h"
#include "../../../runtime/frame.h"

using namespace utf8;

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static void defineClass0(Frame *frame)
{
    auto _this = frame->getLocalAsRef(0);
    auto name = frame->getLocalAsRef(1);
    auto b = frame->getLocalAsRef<Array>(2);
    jint off = frame->getLocalAsInt(3);
    jint len = frame->getLocalAsInt(4);
    auto pd = frame->getLocalAsRef(5);

    frame->pushr(defineClass(_this, name, b, off, len, pd));
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static void defineClass1(Frame *frame)
{
    auto _this = frame->getLocalAsRef(0);
    auto name = frame->getLocalAsRef(1);
    auto b = frame->getLocalAsRef<Array>(2);
    jint off = frame->getLocalAsInt(3);
    jint len = frame->getLocalAsInt(4);
    auto pd = frame->getLocalAsRef(5);
    auto source = frame->getLocalAsRef(6);

    frame->pushr(defineClass(_this, name, b, off, len, pd, source));
}

// private native Class<?> defineClass2(String name,
//                              java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
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

// load bootstrap class
// private native Class<?> findBootstrapClass(String name);
static void findBootstrapClass(Frame *frame)
{
    auto name = frame->getLocalAsRef(1)->toUtf8();
    Class *c = loadBootClass(dots2SlashDup(name));
    frame->pushr((jref)(c));
}

// private native final Class<?> findLoadedClass0(String name);
static void findLoadedClass0(Frame *frame)
{
    auto _this = frame->getLocalAsRef(0);
    auto name = frame->getLocalAsRef(1)->toUtf8();

    Class *c = findLoadedClass(_this, dots2SlashDup(name));
    frame->pushr((jref)(c));
}

// private static native String findBuiltinLib(String name);
static void findBuiltinLib(Frame *frame)
{
    // todo
    auto name0 = frame->getLocalAsRef(0);

    const char *name = name0->toUtf8();
    if (strcmp(name, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = R"(C:\Progles (x86)\Java\jre1.8.0_221\bin\zip.dll)";  // todo
        frame->pushr(newString(buf));    // todo
    } else if (strcmp(name, "management.dll") == 0) {
        frame->pushr(newString("ffffffff"));    // todo
    } else {
        jvm_abort(name); // todo
    }
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
#define C "java/lang/ClassLoader"
    registerNative(C, "defineClass0",
                   "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;)Ljava/lang/Class;", defineClass0);
    registerNative(C, "defineClass1",
                   "(Ljava/lang/String;[BIILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                   defineClass1);
    registerNative(C, "defineClass2",
                   "(Ljava/lang/String;Ljava/nio/ByteBuffer;IILjava/security/ProtectionDomain;Ljava/lang/String;)Ljava/lang/Class;",
                   defineClass2);
    registerNative(C, "resolveClass0", "(Ljava/lang/Class;)V", resolveClass0);
    registerNative(C, "findBootstrapClass", "(Ljava/lang/String;)Ljava/lang/Class;", findBootstrapClass);
    registerNative(C, "findLoadedClass0", "(Ljava/lang/String;)Ljava/lang/Class;", findLoadedClass0);
    registerNative(C, "findBuiltinLib", "(Ljava/lang/String;)Ljava/lang/String;", findBuiltinLib);
    registerNative(C, "retrieveDirectives", "()Ljava/lang/AssertionStatusDirectives;", retrieveDirectives);
}

