/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../interpreter/stack_frame.h"
#include "../../../rtda/heap/jobject.h"

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
    jint offset = slot_geti(get_instance_field_value_by_nt(field_obj, "slot", "I")); // todo "slot", "I" 什么东西
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
#endif
    // todo
    os_pushi(frame->operand_stack, 8); // todo unsafe.Sizeof(int)
}

/*
 * 第一个参数为需要改变的对象，
 * 第二个为偏移量(参加函数 objectFieldOffset)，
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
    //    jref this_obj = slot_getr(frame->local_vars);
    jref o = slot_getr(frame->local_vars + 1); // first argument
    jlong offset = slot_getl(frame->local_vars + 2); // long 占两个Slot
    jint expected = slot_geti(frame->local_vars + 4);
    jint x = slot_geti(frame->local_vars + 5);

    jint value = slot_geti(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = islot(x);
        set_instance_field_value_by_id(o, offset, &s);
        os_pushi(frame->operand_stack, 1); // todo
    } else {
        os_pushi(frame->operand_stack, 0); // todo
    }
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(struct stack_frame *frame)
{
    //    jref this_obj = slot_getr(frame->local_vars);
    jref o = slot_getr(frame->local_vars + 1); // first argument
    jlong offset = slot_getl(frame->local_vars + 2); // long 占两个Slot
    jlong expected = slot_getl(frame->local_vars + 4);
    jlong x = slot_getl(frame->local_vars + 6);

    jlong value = slot_getl(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = lslot(x);
        set_instance_field_value_by_id(o, offset, &s);
        os_pushi(frame->operand_stack, 1); // todo
    } else {
        os_pushi(frame->operand_stack, 0); // todo
    }
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(struct stack_frame *frame)
{
//    jref this_obj = slot_getr(frame->local_vars);
    jref o = slot_getr(frame->local_vars + 1); // first argument
    jlong offset = slot_getl(frame->local_vars + 2); // long 占两个Slot
    jref expected = slot_getr(frame->local_vars + 4);
    jref x = slot_getr(frame->local_vars + 5);

    jref value = slot_getr(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = rslot(x);
        set_instance_field_value_by_id(o, offset, &s);
        os_pushi(frame->operand_stack, 1); // todo
    } else {
        os_pushi(frame->operand_stack, 0); // todo
    }
}

// public native int getIntVolatile(Object o, long offset);
static void getIntVolatile(struct stack_frame *frame)
{
//    jref this_obj = slot_getr(frame->local_vars);
    jref o = slot_getr(frame->local_vars + 1); // first argument
    jlong offset = slot_getl(frame->local_vars + 2);

    jint value;
    if (jclass_is_array(o->jclass)) {
//        value = *((jint *)jarrobj_index(o, offset));
        value = jarrobj_get(jint, o, offset);  // todo
    } else {
        value = slot_geti(get_instance_field_value_by_id(o, offset));  // todo
    }
    os_pushi(frame->operand_stack, value);
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