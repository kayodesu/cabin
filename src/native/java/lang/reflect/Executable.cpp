#include "../../../jni_inner.h"
#include "../../../../runtime/frame.h"
#include "../../../../kayo.h"

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

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getParameters0", "()[Ljava/lang/reflect/Parameter;", (void *) getParameters0 },
        { "getTypeAnnotationBytes0", "()[B", (void *) getTypeAnnotationBytes0 },
};

void java_lang_reflect_Executable_registerNatives()
{
    registerNatives("java/lang/reflect/Executable", methods, ARRAY_LENGTH(methods));
}
