/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../rtda/heap/jobject.h"

// public native int hashCode();
static void hashCode(struct stack_frame *frame)
{
    struct jobject *this_obj = slot_getr(frame->local_vars);
    os_pushi(frame->operand_stack, (jint) this_obj); // todo
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    frame->operandStack.push((jint)((jlong)thisObj)); // todo
}

// protected native Object clone() throws CloneNotSupportedException;
static void clone(struct stack_frame *frame)
{
    jvm_abort("error\n");
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
    struct jobject *this_obj = slot_getr(frame->local_vars);
    os_pushr(frame->operand_stack, this_obj->jclass->clsobj);
//    JObject *thisObj = frame->getLocalVar(0).getRef();
//    JClassObj *classObj = thisObj->getClass()->getClassObj();
//    frame->operandStack.push(classObj);
}

void java_lang_Object_registerNatives()
{
    register_native_method("java/lang/Object", "registerNatives", "()V", empty_method);
    register_native_method("java/lang/Object", "hashCode", "()I", hashCode);
    register_native_method("java/lang/Object", "getClass", "()Ljava/lang/Class;", getClass);
    register_native_method("java/lang/Object", "clone", "()Ljava/lang/Object;", clone);
}
