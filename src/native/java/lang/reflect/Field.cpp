#include "../../../jni_inner.h"
#include "../../../../runtime/frame.h"
#include "../../../../kayo.h"

/*
 * Author: kayo
 */

// private native byte[] getTypeAnnotationBytes0();
static void getTypeAnnotationBytes0(Frame *frame)
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