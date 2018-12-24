#include "../../registry.h"
#include "../../../jtypes.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/jobject.h"
#include "../../../rtda/heap/strpool.h"

/*
 * Author: Jia Yang
 */

// private native long getLongAt0(Object o, int i);
static void getLongAt0(struct frame *frame)
{
    // todo 对不对
    jref o = frame_locals_getr(frame, 1);
    jint i = frame_locals_geti(frame, 2);
    jlong result = rtcp_get_long(o->jclass->rtcp, i);
    frame_stack_pushl(frame, result);
}

// private native String getUTF8At0(Object o, int i);
static void getUTF8At0(struct frame *frame)
{
    // todo 对不对
    jref o = frame_locals_getr(frame, 1);
    jint i = frame_locals_geti(frame, 2);
    jref result = get_str_from_pool(frame->m.method->jclass->loader, rtcp_get_str(o->jclass->rtcp, i));
    frame_stack_pushr(frame, result);
}

void sun_reflect_ConstantPool_registerNatives()
{
    register_native_method("sun/reflect/ConstantPool~registerNatives~()V", registerNatives);
    register_native_method("sun/reflect/ConstantPool~getLongAt0~(Ljava/lang/Object;I)J", getLongAt0);
    register_native_method("sun/reflect/ConstantPool~getUTF8At0~(Ljava/lang/Object;I)Ljava/lang/String;", getUTF8At0);
}
