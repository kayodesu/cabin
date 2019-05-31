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
    jref o = frame_locals_getr(frame, 1);
    jint i = frame_locals_geti(frame, 2);
    jlong result = CP_LONG(&(o->clazz->constant_pool), i);//rtcp_get_long(o->clazz->rtcp, i);
    frame_stack_pushl(frame, result);
}

// private native String getUTF8At0(Object o, int i);
static void getUTF8At0(Frame *frame)
{
    // todo 对不对
    jref o = frame_locals_getr(frame, 1);
    jint i = frame_locals_geti(frame, 2);

//    resolve_single_constant(o->clazz, i);
//    jref result = get_str_from_pool(frame->m.method->clazz->loader, CP_STRING(&(o->clazz->constant_pool), i));//rtcp_get_str(o->clazz->rtcp, i));
    frame_stack_pushr(frame, resolve_string(o->clazz, i));
}

void sun_reflect_ConstantPool_registerNatives()
{
    register_native_method("sun/reflect/ConstantPool", "getLongAt0", "(Ljava/lang/Object;I)J", getLongAt0);
    register_native_method("sun/reflect/ConstantPool", "getUTF8At0", "(Ljava/lang/Object;I)Ljava/lang/String;", getUTF8At0);
}
