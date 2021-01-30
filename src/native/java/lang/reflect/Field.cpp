#include "../../../jni_inner.h"

// private native byte[] getTypeAnnotationBytes0();
static jbyteArray getTypeAnnotationBytes0(jobject _this)
{
    jvm_abort("getTypeAnnotationBytes0"); // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getTypeAnnotationBytes0", "()[B", (void *) getTypeAnnotationBytes0 },
};

void java_lang_reflect_Field_registerNatives()
{
    registerNatives("java/lang/reflect/Field", methods, ARRAY_LENGTH(methods));
}