/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../objects/class_loader.h"
#include "../../../metadata/class.h"
#include "../../../objects/class_object.h"
#include "../../../objects/string_object.h"

using namespace utf8;

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static jclass defineClass0(jobject _this, jstring name, jbyteArray b, jint off, jint len, jobject pd)
{
    return defineClass(_this, name, b, off, len, pd)->java_mirror;
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass1(jobject _this, jstring name,
                        jbyteArray b, jint off, jint len, jobject pd, jstring source)
{
    return defineClass(_this, name, b, off, len, pd, source)->java_mirror;
}

// private native Class<?> defineClass2(String name,
//                              java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass2(jobject _this, 
                jstring name, jobject b, jint off, jint len, jobject pd, jstring source)
{
    // todo
    jvm_abort("defineClass2");
}

// private native void resolveClass0(Class<?> c);
static void resolveClass0(jobject _this, jclass c)
{
    // todo
    jvm_abort("resolveClass0");
}

// load bootstrap class
// private native Class<?> findBootstrapClass(String name);
static jclass findBootstrapClass(jobject _this, jstring name)
{
    auto slash_name = dot2SlashDup(name->toUtf8());
    Class *c = loadBootClass(slash_name);
    return c != nullptr ? c->java_mirror : nullptr;
}

// private native final Class<?> findLoadedClass0(String name);
static jclass findLoadedClass0(jobject _this, jstring name)
{
    auto slash_name = dot2SlashDup(name->toUtf8());
    Class *c = findLoadedClass(_this, slash_name);
    return c != nullptr ? c->java_mirror : nullptr;
}

// private static native String findBuiltinLib(String name);
static jstring findBuiltinLib(jstring name)
{
    const char *utf8_name = name->toUtf8();
    if (equals(utf8_name, "zip.dll")) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = "(C:\\Progles (x86)\\Java\\jre1.8.0_221\\bin\\zip.dll)";  // todo
        return newString(buf); // todo
    } else if (equals(utf8_name, "management.dll")) {
        return newString("ffffffff"); // todo
    } else {
        jvm_abort(utf8_name); // todo
    }
}

// Retrieves the assertion directives from the VM.
// private static native AssertionStatusDirectives retrieveDirectives();
static void retrieveDirectives()
{
    // todo
    jvm_abort("retrieveDirectives");
}

#undef PD
#define PD "Ljava/security/ProtectionDomain;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "defineClass0", "(" STR "[BII" PD ")" CLS, (void *) defineClass0 },
        { "defineClass1", "(" STR "[BII" PD STR ")" CLS, (void *) defineClass1 },
        { "defineClass2", "(" STR "Ljava/nio/ByteBuffer;II" PD STR ")" CLS, (void *) defineClass2 },
        { "resolveClass0", "(Ljava/lang/Class;)V", (void *) resolveClass0 },
        { "findBootstrapClass", _STR_ CLS, (void *) findBootstrapClass },
        { "findLoadedClass0", _STR_ CLS, (void *) findLoadedClass0 },
        { "findBuiltinLib", _STR_ STR, (void *) findBuiltinLib },
        { "retrieveDirectives", "()Ljava/lang/AssertionStatusDirectives;", (void *) retrieveDirectives },
};

void java_lang_ClassLoader_registerNatives()
{
    registerNatives("java/lang/ClassLoader", methods, ARRAY_LENGTH(methods));
}

