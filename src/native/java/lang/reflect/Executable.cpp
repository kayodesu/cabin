#include "../../../jni_inner.h"
#include "../../../../runtime/frame.h"
#include "../../../../kayo.h"

/*
 * Author: Yo Ka
 */

// private native Parameter[] getParameters0();
static jobjectArray getParameters0(JNIEnv *env, jobject _this)
{
    jvm_abort("getParameters0"); // todo
}

// native byte[] getTypeAnnotationBytes0();
static jbyteArray getTypeAnnotationBytes0(JNIEnv *env, jobject _this)
{
    jvm_abort("getTypeAnnotationBytes0"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getParameters0", "()[Ljava/lang/reflect/Parameter;", (void *) getParameters0 },
        { "getTypeAnnotationBytes0", "()[B", (void *) getTypeAnnotationBytes0 },
};

void java_lang_reflect_Executable_registerNatives()
{
    registerNatives("java/lang/reflect/Executable", methods, ARRAY_LENGTH(methods));
}
