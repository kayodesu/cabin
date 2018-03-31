/*
 * Author: Jia Yang
 */

#include "../../../interpreter/StackFrame.h"
#include "../../registry.h"
#include "../../../rtda/heap/objectarea/JarrayObj.h"

// public native int arrayBaseOffset(Class<?> type)
static void arrayBaseOffset(StackFrame *frame) {
    frame->operandStack.push((jint)0);  // todo
}

// public native int arrayIndexScale(Class<?> type)
static void arrayIndexScale(StackFrame *frame) {
    frame->operandStack.push((jint)1);  // todo
}

// public native long objectFieldOffset(Field field)
static void objectFieldOffset(StackFrame *frame) {
    /*
     * 	vars := frame.LocalVars()
	jField := vars.GetRef(1)

	offset := jField.GetFieldValue("slot", "I").(int32)

	stack := frame.OperandStack()
	stack.PushLong(int64(offset))
     */
    Jobject *jlrFieldObj = static_cast<Jobject *>(frame->getLocalVars(1).value.r);
    jint offset = jlrFieldObj->getInstanceFieldValue("slot", "I").i;
    frame->operandStack.push((jlong)offset);
}

// public native int addressSize()
static void addressSize(StackFrame *frame) {
/*
 * 	// vars := frame.LocalVars()
	// vars.GetRef(0) // this

	stack := frame.OperandStack()
	stack.PushInt(8) // todo unsafe.Sizeof(int)
 */
    frame->operandStack.push((jint)8);  // todo
}

/*
 * 第一个参数为需要改变的对象，
 * 第二个为偏移量(即之前求出来的valueOffset的值)，
 * 第三个参数为期待的值，
 * 第四个为更新后的值。
 *
 * 整个方法的作用即为若调用该方法时，value的值与expect这个值相等，那么则将value修改为update这个值，并返回一个true，
 * 如果调用该方法时，value的值与expect这个值不相等，那么不做任何操作，并范围一个false。
 *
 * public final native boolean compareAndSwapInt(Object o, long offset, int expected, int x);
 */
static void compareAndSwapInt(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).value.r);
    Jobject *o = static_cast<Jobject *>(frame->getLocalVars(1).value.r);
    jlong offset = frame->getLocalVars(2).value.l; // long 占两个Slot
    jint expected = frame->getLocalVars(4).value.i;
    jint x = frame->getLocalVars(5).value.i;

    jint value = o->getInstanceFieldValue(offset).i;
    if (value == expected) {
        Jvalue v;
        v.i = x;
        o->setInstanceFieldValue(offset, v);
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).value.r);
    Jobject *o = static_cast<Jobject *>(frame->getLocalVars(1).value.r);
    jlong offset = frame->getLocalVars(2).value.l; // long 占两个Slot
    jlong expected = frame->getLocalVars(4).value.l;
    jlong x = frame->getLocalVars(6).value.l;

    jlong value = o->getInstanceFieldValue(offset).l;
    if (value == expected) {
        Jvalue v;
        v.l = x;
        o->setInstanceFieldValue(offset, v);
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).value.r);
    Jobject *o = static_cast<Jobject *>(frame->getLocalVars(1).value.r);
    jlong offset = frame->getLocalVars(2).value.l; // long 占两个Slot
    jreference expected = frame->getLocalVars(4).value.r;
    jreference x = frame->getLocalVars(5).value.r;

    jreference value = o->getInstanceFieldValue(offset).r;
    if (value == expected) {
        Jvalue v;
        v.r = x;
        o->setInstanceFieldValue(offset, v);
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
}

// public native int getIntVolatile(Object o, long offset);
static void getIntVolatile(StackFrame *frame) {
    Jobject *thisObj = static_cast<Jobject *>(frame->getLocalVars(0).value.r);
    Jobject *o = static_cast<Jobject *>(frame->getLocalVars(1).value.r);
    jlong offset = frame->getLocalVars(2).value.l;

    if (o->getClass()->isArray()) {
        JarrayObj *arr = dynamic_cast<JarrayObj *>(o);
        jint value = arr->get<jint>(offset);
        frame->operandStack.push(value);
    } else {
        jint value = o->getInstanceFieldValue(offset).i;
        frame->operandStack.push(value);
    }
}

void sun_misc_Unsafe_registerNatives() {
    registerNativeMethod("sun/misc/Unsafe", "registerNatives", "()V", [](StackFrame *){});
    registerNativeMethod("sun/misc/Unsafe", "arrayBaseOffset", "(Ljava/lang/Class;)I", arrayBaseOffset);
    registerNativeMethod("sun/misc/Unsafe", "arrayIndexScale", "(Ljava/lang/Class;)I", arrayIndexScale);
    registerNativeMethod("sun/misc/Unsafe", "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", objectFieldOffset);
    registerNativeMethod("sun/misc/Unsafe", "addressSize", "()I", addressSize);

    registerNativeMethod("sun/misc/Unsafe", "compareAndSwapInt", "(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    registerNativeMethod("sun/misc/Unsafe", "compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    registerNativeMethod("sun/misc/Unsafe", "compareAndSwapObject",
                         "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z", compareAndSwapObject);

    registerNativeMethod("sun/misc/Unsafe", "getIntVolatile", "(Ljava/lang/Object;J)I", getIntVolatile);
}