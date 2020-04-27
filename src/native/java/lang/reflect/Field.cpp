#include "../../../jnidef.h"
#include "../../../../vmdef.h"

/*
 * Author: Yo Ka
 */

// private native byte[] getTypeAnnotationBytes0();
static jarrref getTypeAnnotationBytes0(jref _this)
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