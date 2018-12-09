/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../jvm.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../slot.h"
#include "../../../rtda/heap/JObject.h"

// native long find(String name);
static void find(struct stack_frame *frame)
{
    // todo
    jref name = slot_getr(frame->local_vars + 1);
    printvm("-------- %s\n", jstrobj_value(name));
    os_pushl(frame->operand_stack, 1); // todo
}

// native void load(String name, boolean isBuiltin);
static void load(struct stack_frame *frame)
{
    // todo

    jref name = slot_getr(frame->local_vars + 1);
    bool is_builtin = slot_geti(frame->local_vars + 2) == 0 ? false : true;

    printvm("-------- %s, %d\n", jstrobj_value(name), is_builtin);
}


void java_lang_ClassLoader$NativeLibrary_registerNatives()
{
#define R(method, descriptor) register_native_method("java/lang/ClassLoader$NativeLibrary", #method, descriptor, method)

    R(registerNatives, "()V");
    R(find, "(Ljava/lang/String;)J");
    R(load, "(Ljava/lang/String;Z)V");

#undef R
}

