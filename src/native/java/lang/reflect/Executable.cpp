#include "../../../registry.h"
#include "../../../../jvm.h"

/*
 * Author: kayo
 */

// private native Parameter[] getParameters0();
static void getParameters0(Frame *frame)
{
    jvm_abort("getParameters0"); // todo
}

// native byte[] getTypeAnnotationBytes0();
static void getTypeAnnotationBytes0(Frame *frame)
{
    jvm_abort("getTypeAnnotationBytes0"); // todo
}

void java_lang_reflect_Executable_registerNatives()
{
#undef C
#define C "java/lang/reflect/Executable",
    register_native_method(C"getParameters0", "()[Ljava/lang/reflect/Parameter;", getParameters0);
    register_native_method(C"getTypeAnnotationBytes0", "()[B", getTypeAnnotationBytes0);
}
