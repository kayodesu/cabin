#include "../../../registry.h"
#include "../../../../jvm.h"

/*
 * Author: kayo
 */

// private native byte[] getTypeAnnotationBytes0();
static void getTypeAnnotationBytes0(Frame *frame)
{
    jvm_abort("getTypeAnnotationBytes0"); // todo
}

void java_lang_reflect_Field_registerNatives()
{
    register_native_method("java/lang/reflect/Field", "getTypeAnnotationBytes0", "()[B", getTypeAnnotationBytes0);
}