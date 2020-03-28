#include "../../jni_inner.h"
#include "../../../runtime/frame.h"

/*
 * Author: kayo
 */

// private static native URL[] getLookupCacheURLs(ClassLoader var0);
static void getLookupCacheURLs(Frame *frame)
{
    // todo
    frame->pushr(nullptr);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "getLookupCacheURLs", "(Ljava/lang/ClassLoader;)[Ljava/net/URL;", (void *) getLookupCacheURLs },
};

void sun_misc_URLClassPath_registerNatives()
{
    registerNatives("sun/misc/URLClassPath", methods, ARRAY_LENGTH(methods));
}