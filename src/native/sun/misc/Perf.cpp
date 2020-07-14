/*
 * Author: Yo Ka
 */

#include "../../jni_inner.h"
#include "../../../slot.h"
#include "../../../objects/class.h"
#include "../../../runtime/frame.h"
#include "../../../interpreter/interpreter.h"

// public native ByteBuffer createLong(String name, int variability, int units, long value);
static jobject createLong(jobject _this, jstring name, jint variability, jint units, jlong value)
{
    // todo 这函数是干嘛的？？？？？？？？？？？？？？？？？？
/*
 * 	bbClass := frame.ClassLoader().LoadClass("java/nio/ByteBuffer")
	if bbClass.InitializationNotStarted() {
		frame.RevertNextPC()
		frame.Thread().InitClass(bbClass)
		return
	}

	stack := frame.OperandStack()
	stack.PushInt(8)

	allocate := bbClass.GetStaticMethod("allocate", "(I)Ljava/nio/ByteBuffer;")
	frame.Thread().InvokeMethod(allocate)
 */

    Class *bb = loadBootClass("java/nio/ByteBuffer");
    initClass(bb);

    Method *allocate = bb->getDeclaredStaticMethod("allocate", "(I)Ljava/nio/ByteBuffer;");
    return RSLOT(execJavaFunc(allocate, { slot::islot(sizeof(jlong)) }));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "createLong", "(Ljava/lang/String;IIJ)Ljava/nio/ByteBuffer;", (void *) createLong },
};

void sun_misc_Perf_registerNatives()
{
    registerNatives("sun/misc/Perf", methods, ARRAY_LENGTH(methods));
}
