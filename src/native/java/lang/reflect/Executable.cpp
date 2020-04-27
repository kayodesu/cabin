#include "../../../jnidef.h"
#include "../../../../vmdef.h"

/*
 * Author: Yo Ka
 */

// private native Parameter[] getParameters0();
static jarrref getParameters0(jref _this)
{
    jvm_abort("getParameters0"); // todo
}

// native byte[] getTypeAnnotationBytes0();
static jarrref getTypeAnnotationBytes0(jref _this)
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
