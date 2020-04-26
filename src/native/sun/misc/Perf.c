/*
 * Author: Yo Ka
 */

#include "../../jnidef.h"
#include "../../ifn.h"

// public native ByteBuffer createLong(String name, int variability, int units, long value);
static jobject createLong(JNIEnv *env, jobject _this, jstring name, jint variability, jint units, jlong value)
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

    jclass c = (*env)->FindClass(env, "java/nio/ByteBuffer");
    ifn.initClass(c);

    jmethodID m = (*env)->GetStaticMethodID(env, c, "allocate", "(I)Ljava/nio/ByteBuffer;");
    return (*env)->CallStaticObjectMethod(env, c, m, sizeof(jlong));
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "createLong", "(Ljava/lang/String;IIJ)Ljava/nio/ByteBuffer;", (void *) createLong },
};

void sun_misc_Perf_registerNatives()
{
    registerNatives("sun/misc/Perf", methods, ARRAY_LENGTH(methods));
}
