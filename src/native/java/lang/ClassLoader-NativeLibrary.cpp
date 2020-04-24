/*
 * Author: kayo
 */

#include "../../../kayo.h"
#include "../../../runtime/frame.h"
#include "../../jni_inner.h"

// native long find(String name);
static void find(Frame *frame)
{
    // todo
//    jref name = slot_getr(frame->local_vars + 1);
//    printvm("-------- %s\n", jstrobj_value(name));
    frame->pushl(1); // todo
}

// native void load(String name, boolean isBuiltin);
static void load(Frame *frame)
{
    // todo
    jref _this = frame->getLocalAsRef(0);
    jref name = frame->getLocalAsRef(1);
//    bool is_builtin = slot_geti(frame->local_vars + 2) == 0 ? false : true;

    // todo load

    // set boolean loaded is true
    _this->setBoolField("loaded", "Z", jtrue);
}

// native void unload(String name, boolean isBuiltin);
static void unload(Frame *frame)
{
    // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "find", "(Ljava/lang/String;)J", (void *) find },
        { "find", "(Ljava/lang/String;)J", (void *) find },
        { "load", "(Ljava/lang/String;Z)V", (void *) load },
        { "unload", "(Ljava/lang/String;Z)V", (void *) unload }
};

void java_lang_ClassLoader$NativeLibrary_registerNatives()
{
    registerNatives("java/lang/ClassLoader$NativeLibrary", methods, ARRAY_LENGTH(methods));
}
