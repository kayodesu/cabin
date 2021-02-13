#include "../../jni_internal.h"
#include "../../../objects/object.h"
#include "../../../cabin.h"
#include "../../../runtime/frame.h"
#include "../../../runtime/vm_thread.h"
#include "../../../metadata/class.h"
#include "../../../exception.h"

using namespace std;

// public static native Class<?> getCallerClass(int level)
static jclass getCallerClass0(jint level)
{
    if (level < 0) {
        throw java_lang_IllegalArgumentException(to_string(level));
    }

    auto frame = (Frame *) getCurrentThread()->getTopFrame();
    for (; level > 0 && frame != nullptr; level--) {
        frame = frame->prev;
    }

    if (frame == nullptr) {
        return nullptr;
    }
    return frame->method->clazz->java_mirror;
}

// public static native Class<?> getCallerClass()
static jclass getCallerClass()
{
    // top0, current frame is executing getCallerClass()
    // top1, who called getCallerClass, the one who wants to know his caller.
    // top2, the caller of top1, the result.
    Frame *frame = (Frame *) getCurrentThread()->getTopFrame();
    Frame *top1 = frame->prev;
    assert(top1 != nullptr);

    Frame *top2 = top1->prev;
    assert(top2 != nullptr);

    jclass o = top2->method->clazz->java_mirror;
    return o;
}

// public static native int getClassAccessFlags(Class<?> type)
static jint getClassAccessFlags(jclass type)
{
    return type->clazz->access_flags; // todo
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getCallerClass", "()Ljava/lang/Class;", TA(getCallerClass) },
        { "getCallerClass", "(I)Ljava/lang/Class;", TA(getCallerClass0) },
        { "getClassAccessFlags", "(Ljava/lang/Class;)I", TA(getClassAccessFlags) },
};

void sun_reflect_Reflection_registerNatives()
{
    registerNatives("sun/reflect/Reflection", methods, ARRAY_LENGTH(methods));
}