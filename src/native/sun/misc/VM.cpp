/*
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"

// private static native void initialize();
static void initialize(StackFrame *frame) {
    // todo
    Jclass *sysClass = frame->method->jclass->loader->loadClass("java/lang/System");
//    sysClass->clinit(frame);
    Jmethod *m = sysClass->lookupStaticMethod("initializeSystemClass", "()V");
    if (m == nullptr) {

    }
    // todo m == nullptr
    frame->invokeMethod(m);
}

void sun_misc_VM_registerNatives() {
    registerNativeMethod("sun/misc/VM", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("sun/misc/VM", "initialize", "()V", initialize);
}
