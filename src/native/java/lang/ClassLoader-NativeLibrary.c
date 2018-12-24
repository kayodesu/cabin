/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../rtda/thread/frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/jobject.h"

// native long find(String name);
static void find(struct frame *frame)
{
    // todo
//    jref name = slot_getr(frame->local_vars + 1);
//    printvm("-------- %s\n", jstrobj_value(name));
    frame_stack_pushl(frame, 1); // todo
}

// native void load(String name, boolean isBuiltin);
static void load(struct frame *frame)
{
    // todo
    jref this = frame_locals_getr(frame, 0);
    jref name = frame_locals_getr(frame, 1);
//    bool is_builtin = slot_geti(frame->local_vars + 2) == 0 ? false : true;

    // todo load

    // set boolean loaded is true
    struct slot s = islot(1);
    set_instance_field_value_by_nt(this, "loaded", "Z", &s);

//    printvm("-------- %s, %d\n", jstrobj_value(name), is_builtin);
}


void java_lang_ClassLoader$NativeLibrary_registerNatives()
{
    register_native_method("java/lang/ClassLoader$NativeLibrary~registerNatives~()V", registerNatives);
    register_native_method("java/lang/ClassLoader$NativeLibrary~find~(Ljava/lang/String;)J", find);
    register_native_method("java/lang/ClassLoader$NativeLibrary~load~(Ljava/lang/String;Z)V", load);
}

