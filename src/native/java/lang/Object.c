/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// public native int hashCode();
static void hashCode(struct stack_frame *frame)
{
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    frame->operandStack.push((jint)((jlong)thisObj)); // todo
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone0(struct stack_frame *frame)
{
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    JClass *cloneable = frame->method->jclass->loader->loadClass("java/lang/Cloneable");
//
//    if (!(thisObj->getClass()->isSubclassOf(cloneable))) {  // todo
//        jvmAbort("java.lang.CloneNotSupportedException");
//    }
//
//    frame->operandStack.push(thisObj->clone());
}

// public final native Class<?> getClass();
static void getClass(struct stack_frame *frame)
{
    // todo
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    JClassObj *classObj = thisObj->getClass()->getClassObj();
//    frame->operandStack.push(classObj);
}

void java_lang_Object_registerNatives(struct stack_frame *frame)
{
    register_native_method("java/lang/Object", "hashCode", "()I", hashCode);
    register_native_method("java/lang/Object", "getClass", "()Ljava/lang/Class;", getClass);
    register_native_method("java/lang/Object", "clone", "()Ljava/lang/Object;", clone0);
}
