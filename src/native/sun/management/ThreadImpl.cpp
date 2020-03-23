/*
 * Author: kayo
 */

#include "../../../runtime/frame.h"
#include "../../../runtime/thread_info.h"
#include "../../../objects/array_object.h"

/*
 * VM support where maxDepth == -1 to request entire stack dump
 *
 * private static native ThreadInfo[] dumpThreads0(
 *                      long[] ids, boolean lockedMonitors, boolean lockedSynchronizers, int maxDepth);
 *
 */
static void dumpThreads0(Frame *frame)
{
    auto ids = frame->getLocalAsRef<Array>(0);
    jbool lockedMonitors = frame->getLocalAsBool(1);
    jbool lockedSynchronizers = frame->getLocalAsBool(2);
    jint maxDepth = frame->getLocalAsInt(3);

    jarrref threadInfos;

    if (ids == jnull) { // dump all threads
        int len = g_all_threads.size();
        threadInfos = newArray(loadBootClass("[Ljava/lang/management/ThreadInfo;"), len);

        for (int i = 0; i < len; i++) {
            Thread *t = g_all_threads[i];
            jref threadInfo = t->to_java_lang_management_ThreadInfo(lockedMonitors, lockedSynchronizers, maxDepth);
            threadInfos->set(i, threadInfo);
        }
    } else {
        threadInfos = newArray(loadBootClass("[Ljava/lang/management/ThreadInfo;"), ids->len);

        for (int i = 0; i < ids->len; i++) {
            auto id = ids->get<jlong>(i);
            Thread *t = Thread::from(id);
            assert(t != nullptr);
            jref threadInfo = t->to_java_lang_management_ThreadInfo(lockedMonitors, lockedSynchronizers, maxDepth);
            threadInfos->set(i, threadInfo);
        }
    }

    frame->pushr(threadInfos);
}

void sun_management_ThreadImpl_registerNatives()
{
#undef C
#define C "sun/management/ThreadImpl"
    registerNative(C, "dumpThreads0", "([JZZ)[Ljava/lang/management/ThreadInfo;", dumpThreads0);
}