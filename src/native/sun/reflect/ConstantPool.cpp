#include "../../registry.h"
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

void sun_reflect_ConstantPool_registerNatives()
{
#undef C
#define C "sun/reflect/ConstantPool"
    registerNative(C, "getSize0", "(Ljava/lang/Object;)I", getSize0);
    registerNative(C, "getClassAt0", "(Ljava/lang/Object;I)Ljava/lang/Class;", getClassAt0);
    registerNative(C, "getLongAt0", "(Ljava/lang/Object;I)J", getLongAt0);
    registerNative(C, "getUTF8At0", "(Ljava/lang/Object;I)Ljava/lang/String;", getUTF8At0);
}
