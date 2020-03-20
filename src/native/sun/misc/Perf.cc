/*
 * Author: kayo
 */

#include "../../registry.h"
#include "../../../objects/class.h"
#include "../../../runtime/Frame.h"
#include "../../../interpreter/interpreter.h"

// public native ByteBuffer createLong(String name, int variability, int units, long value);
static void createLong(Frame *frame)
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
//    bb->clinit();

    Method *allocate = bb->getDeclaredStaticMethod("allocate", "(I)Ljava/nio/ByteBuffer;");
    frame->pushr(RSLOT(execJavaFunc(allocate, { to_islot(sizeof(jlong)) })));
}

void sun_misc_Perf_registerNatives()
{
#undef C
#define C "sun/misc/Perf"
    registerNative(C, "createLong", "(Ljava/lang/String;IIJ)Ljava/nio/ByteBuffer;", createLong);
}
