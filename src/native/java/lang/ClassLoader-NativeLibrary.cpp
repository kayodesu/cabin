#include "../../jni_internal.h"
#include "../../../objects/object.h"

// native long find(String name);
static jlong find(jobject _this, jstring name)
{
//    printvm("-------- %s\n", jstrobj_value(name));
    return 1; // todo
}

// native void load(String name, boolean isBuiltin);
static void load(jobject _this, jstring name, jboolean isBuiltin)
{
    // todo load

    // set boolean loaded is true
    _this->setBoolField("loaded", "Z", jtrue);

//    jclass c = (*env)->GetObjectClass(env, _this);
//    jfieldID id = (*env)->GetFieldID(env, c, "loaded", "Z");
//    (*env)->SetBooleanField(env, _this, id, jtrue);
}

// native void unload(String name, boolean isBuiltin);
static void unload(jobject _this, jstring name, jboolean isBuiltin)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "find", "(Ljava/lang/String;)J", TA(find) },
        { "find", "(Ljava/lang/String;)J", TA(find) },
        { "load", "(Ljava/lang/String;Z)V", TA(load) },
        { "unload", "(Ljava/lang/String;Z)V", TA(unload) },
};

void java_lang_ClassLoader$NativeLibrary_registerNatives()
{
    registerNatives("java/lang/ClassLoader$NativeLibrary", methods, ARRAY_LENGTH(methods));
}
