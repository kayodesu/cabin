#include "../../registry.h"
#include "../../../jtypes.h"
#include "../../../rtda/thread/Frame.h"
#include "../../../rtda/heap/Object.h"
#include "../../../rtda/heap/StrPool.h"
#include "../../../rtda/ma/resolve.h"

/*
 * Author: Jia Yang
 */

// private native long getLongAt0(Object o, int i);
static void getLongAt0(Frame *frame)
{
    // todo 对不对
    jref o = frame->getLocalAsRef(1);
    jint i = frame->getLocalAsInt(2);

    frame->pushl(CP_LONG(o->clazz->cp, i));
}

// private native String getUTF8At0(Object o, int i);
static void getUTF8At0(Frame *frame)
{
    // todo 对不对
    jref o = frame->getLocalAsRef(1);
    jint i = frame->getLocalAsInt(2);

    frame->pushr(resolve_string(o->clazz, i));
}

void sun_reflect_ConstantPool_registerNatives()
{
    register_native_method("sun/reflect/ConstantPool", "getLongAt0", "(Ljava/lang/Object;I)J", getLongAt0);
    register_native_method("sun/reflect/ConstantPool", "getUTF8At0", "(Ljava/lang/Object;I)Ljava/lang/String;", getUTF8At0);
}
