/*
 * Author: Yo Ka
 */

#include "../../../kayo.h"
#include "../../../objects/class.h"
#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

using namespace utf8;

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static jclass defineClass0(JNIEnv *env, jref _this, jstrref name, jarrref b, jint off, jint len, jref pd)
{
    return to_jclass(defineClass(_this, name, b, off, len, pd));
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass1(JNIEnv *env, jref _this, jstrref name, jarrref b, jint off, jint len, jref pd, jstrref source)
{
    return to_jclass(defineClass(_this, name, b, off, len, pd, source));
}

// private native Class<?> defineClass2(String name,
//                              java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass2(JNIEnv *env, jref _this, jstrref name, jref b, jint off, jint len, jref pd, jstrref source)
{
    // todo
    jvm_abort("defineClass2");
    return nullptr;
}

// private native void resolveClass0(Class<?> c);
static void resolveClass0(JNIEnv *env, jref _this, jclass c)
{
    // todo
    jvm_abort("resolveClass0");
}

// load bootstrap class
// private native Class<?> findBootstrapClass(String name);
static jclass findBootstrapClass(JNIEnv *env, jref _this, jstrref name)
{
    Class *c = loadBootClass(dots2SlashDup(name->toUtf8()));
    return to_jclass(c);
}

// private native final Class<?> findLoadedClass0(String name);
static jclass findLoadedClass0(JNIEnv *env, jref _this, jstrref name)
{
    Class *c = findLoadedClass(_this, dots2SlashDup(name->toUtf8()));
    return to_jclass(c);
}

// private static native String findBuiltinLib(String name);
static jstrref findBuiltinLib(JNIEnv *env, jclass clazz, jstrref name)
{
    const char *name0 = name->toUtf8();
    if (strcmp(name0, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = R"(C:\Progles (x86)\Java\jre1.8.0_221\bin\zip.dll)";  // todo
        return newString(buf); // todo
    } else if (strcmp(name0, "management.dll") == 0) {
        return newString("ffffffff"); // todo
    } else {
        jvm_abort(name0); // todo
    }
}

// Retrieves the assertion directives from the VM.
// private static native AssertionStatusDirectives retrieveDirectives();
static void retrieveDirectives(JNIEnv *env, jclass clazz)
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

