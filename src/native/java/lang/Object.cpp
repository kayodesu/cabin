/*
 * 实现 java/lang/Object 类下的 native方法
 *
 * Author: Jia Yang
 */


#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/methodarea/Jmethod.h"


// public native int hashCode();
static void hashCode(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).getReference());
    frame->operandStack.push((jint)((jlong)thisObj)); // todo
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone0(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).getReference());
    Jclass *cloneable = frame->method->jclass->loader->loadClass("java/lang/Cloneable");

    if (!(thisObj->getClass()->isSubclassOf(cloneable))) {  // todo
        jvmAbort("java.lang.CloneNotSupportedException");
    }

    frame->operandStack.push(thisObj->clone());
}

// public final native Class<?> getClass();
static void getClass(StackFrame *frame) {
    // todo
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).getReference());
    JclassObj *classObj = thisObj->getClass()->getClassObj();
    frame->operandStack.push(classObj);
}

void java_lang_Object_registerNatives() {
    registerNativeMethod("java/lang/Object", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("java/lang/Object", "hashCode", "()I", hashCode);
    registerNativeMethod("java/lang/Object", "getClass", "()Ljava/lang/Class;", getClass);
    registerNativeMethod("java/lang/Object", "clone", "()Ljava/lang/Object;", clone0);
}
