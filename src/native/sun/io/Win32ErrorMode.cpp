#include "../../jnidef.h"
#include "../../../vmdef.h"

/*
 * Author: Yo Ka
 */

// todo 函数声明，是不是static的.
static jlong setErrorMode(jclsref clazz, jlong l)
{
    return 0; // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "setErrorMode", "(J)J", (void *) setErrorMode },
};

void sun_io_Win32ErrorMode_registerNatives()
{
    registerNatives("sun/io/Win32ErrorMode", methods, ARRAY_LENGTH(methods));
}
