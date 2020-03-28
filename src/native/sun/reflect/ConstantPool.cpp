#include "../../jni_inner.h"
#include "../../../vmdef.h"
#include "../../../runtime/frame.h"
#include "../../../objects/class.h"

/*
 * Author: kayo
 */

// private native int getSize0(Object constantPoolOop);
static void getSize0(Frame *frame)
{
    auto cp = frame->getLocalAsRef<ConstantPool>(1);
    frame->pushi(cp->size);
}

// private native Class getClassAt0(Object constantPoolOop, int i);
static void getClassAt0(Frame *frame)
{
    auto cp = frame->getLocalAsRef<ConstantPool>(1);
    jint i = frame->getLocalAsInt(2);

    frame->pushr(cp->resolveClass((u2)i));
}

// private native long getLongAt0(Object constantPoolOop, int i);
static void getLongAt0(Frame *frame)
{
    auto cp = frame->getLocalAsRef<ConstantPool>(1);
    jint i = frame->getLocalAsInt(2);

    frame->pushl(cp->_long((u2)i));
}

// private native String getUTF8At0(Object constantPoolOop, int i);
static void getUTF8At0(Frame *frame)
{
    auto cp = frame->getLocalAsRef<ConstantPool>(1);
    jint i = frame->getLocalAsInt(2);

    frame->pushr(cp->resolveString(i));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getSize0", "(Ljava/lang/Object;)I", (void *) getSize0 },
        { "getClassAt0", "(Ljava/lang/Object;I)Ljava/lang/Class;", (void *) getClassAt0 },
        { "getLongAt0", "(Ljava/lang/Object;I)J", (void *) getLongAt0 },
        { "getUTF8At0", "(Ljava/lang/Object;I)Ljava/lang/String;", (void *) getUTF8At0 },
};

void sun_reflect_ConstantPool_registerNatives()
{
    registerNatives("sun/reflect/ConstantPool", methods, ARRAY_LENGTH(methods));
}
