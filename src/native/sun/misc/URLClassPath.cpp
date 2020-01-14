#include "../../registry.h"
#include "../../../runtime/Frame.h"

/*
 * Author: kayo
 */

// private static native URL[] getLookupCacheURLs(ClassLoader var0);
static void getLookupCacheURLs(Frame *frame)
{
    // todo
    frame->pushr(nullptr);
}

void sun_misc_URLClassPath_registerNatives()
{
    registerNative("sun/misc/URLClassPath",
                   "getLookupCacheURLs", "(Ljava/lang/ClassLoader;)[Ljava/net/URL;", getLookupCacheURLs);
}