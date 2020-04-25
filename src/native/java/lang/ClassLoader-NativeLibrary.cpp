/*
 * Author: Yo Ka
 */

#include "../../../kayo.h"
#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

// native long find(String name);
static jlong find(JNIEnv *env, jobject _this, jstring name)
{
    // todo
//    jref name = slot_getr(frame->local_vars + 1);
//    printvm("-------- %s\n", jstrobj_value(name));
    return 1; // todo
}

// native void load(String name, boolean isBuiltin);
static void load(JNIEnv *env, jobject _this, jstring name, jboolean isBuiltin)
{
    // todo load

    // set boolean loaded is true
    jclass c = env->GetObjectClass(_this);
    jfieldID id = env->GetFieldID(c, "loaded", "Z");
    env->SetBooleanField(_this, id, jtrue);
}

// native void unload(String name, boolean isBuiltin);
static void unload(JNIEnv *env, jobject _this, jstring name, jboolean isBuiltin)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "find", "(Ljava/lang/String;)J", (void *) find },
        { "find", "(Ljava/lang/String;)J", (void *) find },
        { "load", "(Ljava/lang/String;Z)V", (void *) load },
        { "unload", "(Ljava/lang/String;Z)V", (void *) unload }
};

void java_lang_ClassLoader$NativeLibrary_registerNatives()
{
    registerNatives("java/lang/ClassLoader$NativeLibrary", methods, ARRAY_LENGTH(methods));
}
