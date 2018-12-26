#include "../../../registry.h"
#include "../../../../jvm.h"

/*
 * Author: Jia Yang
 */

// private native Parameter[] getParameters0();
static void getParameters0(struct frame *frame)
{
    jvm_abort(""); // todo
}

// native byte[] getTypeAnnotationBytes0();
static void getTypeAnnotationBytes0(struct frame *frame)
{
    jvm_abort(""); // todo
}

void java_lang_reflect_Executable_registerNatives()
{
    register_native_method("java/lang/reflect/Executable~registerNatives~()V", registerNatives);
    register_native_method("java/lang/reflect/Executable~getParameters0~()[Ljava/lang/reflect/Parameter;", getParameters0);
    register_native_method("java/lang/reflect/Executable~getTypeAnnotationBytes0~()[B", getTypeAnnotationBytes0);
}
