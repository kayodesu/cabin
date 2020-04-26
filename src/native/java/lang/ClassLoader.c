/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../ifn.h"

// private native Class<?> defineClass0(String name, byte[] b, int off, int len, ProtectionDomain pd);
static jclass defineClass0(JNIEnv *env, jobject _this, jstring name,
                        jbyteArray b, jint off, jint len, jobject pd)
{
    return ifn.defineClass0(_this, name, b, off, len, pd);
}

// private native Class<?> defineClass1(String name, byte[] b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass1(JNIEnv *env, jobject _this, jstring name, 
                        jbyteArray b, jint off, jint len, jobject pd, jstring source)
{
    return ifn.defineClass1(_this, name, b, off, len, pd, source);
}

// private native Class<?> defineClass2(String name,
//                              java.nio.ByteBuffer b, int off, int len, ProtectionDomain pd, String source);
static jclass defineClass2(JNIEnv *env, jobject _this, 
                jstring name, jobject b, jint off, jint len, jobject pd, jstring source)
{
    // todo
    jvm_abort("defineClass2");
}

// private native void resolveClass0(Class<?> c);
static void resolveClass0(JNIEnv *env, jobject _this, jclass c)
{
    // todo
    jvm_abort("resolveClass0");
}

// load bootstrap class
// private native Class<?> findBootstrapClass(String name);
static jclass findBootstrapClass(JNIEnv *env, jobject _this, jstring name)
{
    const char *utf8_name = (*env)->GetStringUTFChars(env, name, NULL);
    return ifn.loadBootClassDot(utf8_name);
}

// private native final Class<?> findLoadedClass0(String name);
static jclass findLoadedClass0(JNIEnv *env, jobject _this, jstring name)
{
    const char *utf8_name = (*env)->GetStringUTFChars(env, name, NULL);
    return ifn.findLoadedClassDot(_this, utf8_name);
}

// private static native String findBuiltinLib(String name);
static jstring findBuiltinLib(JNIEnv *env, jclass clazz, jstring name)
{
    const char *utf8_name = (*env)->GetStringUTFChars(env, name, NULL);
    if (strcmp(utf8_name, "zip.dll") == 0) {
        // C:\Program Files\Java\jre1.8.0_162\bin
//        char buf[1024] = R"(C:\Program Files\Java\jre1.8.0_162\bin\zip.dll)";  // todo
        char buf[1024] = "(C:\\Progles (x86)\\Java\\jre1.8.0_221\\bin\\zip.dll)";  // todo
        return (*env)->NewStringUTF(env, buf); // todo
    } else if (strcmp(utf8_name, "management.dll") == 0) {
        return  (*env)->NewStringUTF(env, "ffffffff"); // todo
    } else {
        jvm_abort(utf8_name); // todo
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

