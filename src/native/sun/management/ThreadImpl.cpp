/*
 * Author: Yo Ka
 */

#include "../../../runtime/frame.h"
#include "../../../runtime/thread_info.h"
#include "../../../objects/array_object.h"
#include "../../jni_inner.h"

/*
 * VM support where maxDepth == -1 to request entire stack dump
 *
 * private static native ThreadInfo[] dumpThreads0(
 *                      long[] ids, boolean lockedMonitors, boolean lockedSynchronizers, int maxDepth);
 *
 */
static jobjectArray dumpThreads0(jlongArray ids, jboolean lockedMonitors, jboolean lockedSynchronizers, jint maxDepth)
{
    jobjectArray thread_infos;

    if (ids == jnull) { // dump all threads
        int len = g_all_threads.size();
        thread_infos = newArray(loadBootClass("[Ljava/lang/management/ThreadInfo;"), len);

        for (int i = 0; i < len; i++) {
            Thread *t = g_all_threads[i];
            Object *thread_info = t->to_java_lang_management_ThreadInfo(lockedMonitors, lockedSynchronizers, maxDepth);
            thread_infos->setRef(i, thread_info);
        }
    } else {
        thread_infos = newArray(loadBootClass("[Ljava/lang/management/ThreadInfo;"), ids->len);

        for (int i = 0; i < ids->len; i++) {
            auto id = ids->get<jlong>(i);
            Thread *t = Thread::from(id);
            assert(t != nullptr);
            Object *thread_info = t->to_java_lang_management_ThreadInfo(lockedMonitors, lockedSynchronizers, maxDepth);
            thread_infos->setRef(i, thread_info);
        }
    }

    return thread_infos;
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "dumpThreads0", "([JZZ)[Ljava/lang/management/ThreadInfo;", (void *) dumpThreads0 },
};

void sun_management_ThreadImpl_registerNatives()
{
    registerNatives("sun/management/ThreadImpl", methods, ARRAY_LENGTH(methods));
}