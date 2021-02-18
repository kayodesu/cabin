#include "../../jni_internal.h"
#include "../../../objects/object.h"
#include "../../../objects/array.h"

/*
 * Sets the given stack trace elements with the backtrace of the given Throwable.
 *
 * private static native void initStackTraceElements(StackTraceElement[] elements, Throwable x);
 */
static void initStackTraceElements(jref elements0, jref x)
{
    assert(elements0 != nullptr && elements0->isArrayObject());
    assert(x != nullptr);

    jref backtrace0 = x->getRefField(S(backtrace), S(sig_java_lang_Object));
    if (!backtrace0->isArrayObject()) {
        JVM_PANIC("error");
    }

    auto backtrace = dynamic_cast<Array *>(backtrace0);
    auto elements = dynamic_cast<Array *>(elements0);
    assert(elements->arr_len <= backtrace->arr_len);
    memcpy(elements->data, backtrace->data, elements->arr_len*sizeof(jref));
}

/*
 * Sets the given stack trace element with the given StackFrameInfo
 *
 * private static native void initStackTraceElement(StackTraceElement element, StackFrameInfo sfi);
 */
static void initStackTraceElement(jref elements, jref sfi)
{
    // todo
    JVM_PANIC("initStackTraceElement");
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "initStackTraceElements", "([Ljava/lang/StackTraceElement;Ljava/lang/Throwable;)V", TA(initStackTraceElements) },
        { "initStackTraceElement", "(Ljava/lang/StackTraceElement;Ljava/lang/StackFrameInfo;)V", TA(initStackTraceElement) },
};

void java_lang_StackTraceElement_registerNatives()
{
    registerNatives("java/lang/StackTraceElement", methods, ARRAY_LENGTH(methods));
}
