/*
 * Author: Jia Yang
 */

#include "../../registry.h"

// public native int arrayBaseOffset(Class<?> type)
static void arrayBaseOffset(struct stack_frame *frame)
{
    os_pushi(frame->operand_stack, 0);//frame->operandStack.push((jint)0);  // todo
}

// public native int arrayIndexScale(Class<?> type)
static void arrayIndexScale(struct stack_frame *frame)
{
    os_pushi(frame->operand_stack, 1);//frame->operandStack.push((jint)1);  // todo
}

// public native long objectFieldOffset(Field field)
static void objectFieldOffset(struct stack_frame *frame)
{
    /*
     * 	vars := frame.LocalVars()
	jField := vars.GetRef(1)

	offset := jField.GetFieldValue("slot", "I").(int32)

	stack := frame.OperandStack()
	stack.PushLong(int64(offset))
     */

//    JObject *jlrFieldObj = frame->getLocalVar(1).getRef();
//    jint offset = jlrFieldObj->getFieldValue("slot", "I").getInt();
//    frame->operandStack.push((jlong)offset);

    jref field_obj = slot_getr(frame->local_vars + 1);
    jint offset = slot_geti(get_instance_field_value_by_nt(field_obj, "slot", "I"));
    os_pushl(frame->operand_stack, offset);
}

// public native int addressSize()
static void addressSize(struct stack_frame *frame)
{
#if 0
/*
 * 	// vars := frame.LocalVars()
	// vars.GetRef(0) // this

	stack := frame.OperandStack()
	stack.PushInt(8) // todo unsafe.Sizeof(int)
 */
    frame->operandStack.push((jint)8);  // todo
#endif
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
static void compareAndSwapInt(struct stack_frame *frame)
{
#if 0
    JObject *thisObj = frame->getLocalVar(0).getRef();
    JObject *o = frame->getLocalVar(1).getRef();
    jlong offset = frame->getLocalVar(2).getLong(); // long 占两个Slot
    jint expected = frame->getLocalVar(4).getInt();
    jint x = frame->getLocalVar(5).getInt();

    jint value = o->getFieldValue(offset).getInt();
    if (value == expected) {
        o->setFieldValue(offset, Slot(x));
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
#endif
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(struct stack_frame *frame)
{
#if 0
    JObject *thisObj = frame->getLocalVar(0).getRef();
    JObject *o = frame->getLocalVar(1).getRef();
    jlong offset = frame->getLocalVar(2).getLong(); // long 占两个Slot
    jlong expected = frame->getLocalVar(4).getLong();
    jlong x = frame->getLocalVar(6).getLong();

    jlong value = o->getFieldValue(offset).getLong();
    if (value == expected) {
        o->setFieldValue(offset, Slot(x));
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
#endif
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(struct stack_frame *frame)
{
#if 0
    JObject *thisObj = frame->getLocalVar(0).getRef();
    JObject *o = frame->getLocalVar(1).getRef();
    jlong offset = frame->getLocalVar(2).getLong(); // long 占两个Slot
    jref expected = frame->getLocalVar(4).getRef();
    jref x = frame->getLocalVar(5).getRef();

    jref value = o->getFieldValue(offset).getRef();
    if (value == expected) {
        o->setFieldValue(offset, Slot(x));
        frame->operandStack.push(1); // todo
    } else {
        frame->operandStack.push(0); // todo
    }
#endif
}

// public native int getIntVolatile(Object o, long offset);
static void getIntVolatile(struct stack_frame *frame)
{
#if 0
    JObject *thisObj = frame->getLocalVar(0).getRef();
    JObject *o = frame->getLocalVar(1).getRef();
    jlong offset = frame->getLocalVar(2).getLong();

    if (o->getClass()->isArray()) {
        JArrayObj *arr = dynamic_cast<JArrayObj *>(o);
        jint value = arr->get<jint>(offset);
        frame->operandStack.push(value);
    } else {
        jint value = o->getFieldValue(offset).getInt();
        frame->operandStack.push(value);
    }
#endif
}

void sun_misc_Unsafe_registerNatives()
{
    register_native_method("sun/misc/Unsafe", "registerNatives", "()V", empty_method);
    register_native_method("sun/misc/Unsafe", "arrayBaseOffset", "(Ljava/lang/Class;)I", arrayBaseOffset);
    register_native_method("sun/misc/Unsafe", "arrayIndexScale", "(Ljava/lang/Class;)I", arrayIndexScale);
    register_native_method("sun/misc/Unsafe", "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", objectFieldOffset);
    register_native_method("sun/misc/Unsafe", "addressSize", "()I", addressSize);

    register_native_method("sun/misc/Unsafe", "compareAndSwapInt", "(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    register_native_method("sun/misc/Unsafe", "compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    register_native_method("sun/misc/Unsafe", "compareAndSwapObject",
                         "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z", compareAndSwapObject);

    register_native_method("sun/misc/Unsafe", "getIntVolatile", "(Ljava/lang/Object;J)I", getIntVolatile);
}