#include "../../../registry.h"
#include "../../../../jvm.h"

/*
 * Author: Jia Yang
 */

// private native byte[] getTypeAnnotationBytes0();
static void getTypeAnnotationBytes0(Frame *frame)
{
    jvm_abort(""); // todo
}

void java_lang_reflect_Field_registerNatives()
{
    register_native_method("java/lang/reflect/Field", "getTypeAnnotationBytes0", "()[B", getTypeAnnotationBytes0);
}