#include "../../../jni_internal.h"

// private native Parameter[] getParameters0();
static jobject getParameters0(jobject _this)
{
    JVM_PANIC("getParameters0"); // todo
}

// native byte[] getTypeAnnotationBytes0();
static jobject getTypeAnnotationBytes0(jobject _this)
{
    JVM_PANIC("getTypeAnnotationBytes0"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getParameters0", "()[Ljava/lang/reflect/Parameter;", TA(getParameters0) },
        { "getTypeAnnotationBytes0", "()[B", TA(getTypeAnnotationBytes0) },
};

void java_lang_reflect_Executable_registerNatives()
{
    registerNatives("java/lang/reflect/Executable", methods, ARRAY_LENGTH(methods));
}
