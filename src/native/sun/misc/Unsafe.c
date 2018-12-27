/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/object.h"
#include "../../../util/bigendian.h"
#include "../../../rtda/heap/arrobj.h"

/* todo
http://www.docjar.com/docs/api/sun/misc/Unsafe.html#park%28boolean,%20long%29
Block current thread, returning when a balancing
unpark occurs, or a balancing unpark has
already occurred, or the thread is interrupted, or, if not
absolute and time is not zero, the given time nanoseconds have
elapsed, or if absolute, the given deadline in milliseconds
since Epoch has passed, or spuriously (i.e., returning for no
"reason"). Note: This operation is in the Unsafe class only
because unpark is, so it would be strange to place it
elsewhere.
*/

// public native void park(boolean isAbsolute, long time);
static void park(struct frame *frame)
{
    jvm_abort("");
}

//  public native void unpark(Object thread);
static void unpark(struct frame *frame)
{
    jvm_abort("");
}

/*************************************    compare and swap    ************************************/

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
static void compareAndSwapInt(struct frame *frame)
{
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2); // long 占两个Slot
    jint expected = frame_locals_geti(frame, 4);
    jint new = frame_locals_geti(frame, 5);

    jint old;
    if (jobject_is_array(o)) {
        old = jarrobj_get(jint, o, offset);
    } else {
        old = slot_geti(get_instance_field_value_by_id(o, offset));
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, new);
    frame_stack_pushi(frame, b ? 1 : 0);
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(struct frame *frame)
{
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2); // long 占两个Slot
    jlong expected = frame_locals_getl(frame, 4);
    jlong new = frame_locals_getl(frame, 6);

    jlong old;
    if (jobject_is_array(o)) {
        old = jarrobj_get(jlong, o, offset);
    } else {
        old = slot_getl(get_instance_field_value_by_id(o, offset));
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, new);
    frame_stack_pushi(frame, b ? 1 : 0);
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(struct frame *frame)
{
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2); // long 占两个Slot
    jref expected = frame_locals_getr(frame, 4);
    jref new = frame_locals_getr(frame, 5);

    jref old;
    if (jobject_is_array(o)) {
        old = jarrobj_get(jref, o, offset);
    } else {
        old = slot_getr(get_instance_field_value_by_id(o, offset));
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, new);
    frame_stack_pushi(frame, b ? 1 : 0);
}

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static void allocateInstance(struct frame *frame)
{
    jvm_abort("");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static void defineClass(struct frame *frame)
{
    jvm_abort("");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(struct frame *frame)
{
    jvm_abort("");
}

// public native Object staticFieldBase(Field f);
// public native long staticFieldOffset(Field f);
static void staticFieldOffset(struct frame *frame)
{
    jvm_abort("");
}

static void staticFieldBase(struct frame *frame)
{
    jvm_abort("");
}
/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static void arrayBaseOffset(struct frame *frame)
{
    frame_stack_pushi(frame, 0);//frame->operandStack.push((jint)0);  // todo
}

// public native int arrayIndexScale(Class<?> type)
static void arrayIndexScale(struct frame *frame)
{
    frame_stack_pushi(frame, 1);//frame->operandStack.push((jint)1);  // todo
}

// public native long objectFieldOffset(Field field)
static void objectFieldOffset(struct frame *frame)
{
    /*
     * 	vars := frame.LocalVars()
	jField := vars.GetRef(1)

	offset := jField.GetFieldValue("slot~I").(int32)

	stack := frame.OperandStack()
	stack.PushLong(int64(offset))
     */

//    JObject *jlrFieldObj = frame->getLocalVar(1).getRef();
//    jint offset = jlrFieldObj->getFieldValue("slot", "I").getInt();
//    frame->operandStack.push((jlong)offset);

    jref field_obj = frame_locals_getr(frame, 1);
    jint offset = slot_geti(get_instance_field_value_by_nt(field_obj, "slot", "I")); // todo "slot", "I" 什么东西
//    printvm("-------   %s, %d\n", jobject_to_string(field_obj), offset);
    frame_stack_pushl(frame, offset);
}

// public native boolean getBoolean(Object o, long offset);
static void getBoolean(struct frame *frame)
{
    jvm_abort("");
}

// public native void putBoolean(Object o, long offset, boolean x);
static void putBoolean(struct frame *frame)
{
    jvm_abort("");
}

// public native byte getByte(Object o, long offset);
static void obj_getByte(struct frame *frame)
{
    jvm_abort("");
}

// public native void putByte(Object o, long offset, byte x);
static void obj_putByte(struct frame *frame)
{
    jvm_abort("");
}

// public native char getChar(Object o, long offset);
static void obj_getChar(struct frame *frame)
{
    jvm_abort("");
}

// public native void putChar(Object o, long offset, char x);
static void obj_putChar(struct frame *frame)
{
    jvm_abort("");
}

// public native short getShort(Object o, long offset);
static void obj_getShort(struct frame *frame)
{
    jvm_abort("");
}

// public native void putShort(Object o, long offset, short x);
static void obj_putShort(struct frame *frame)
{
    jvm_abort("");
}

// public native int getInt(Object o, long offset);
static void obj_getInt(struct frame *frame)
{
    jvm_abort("");
}

// public native void putInt(Object o, long offset, int x);
static void obj_putInt(struct frame *frame)
{
    jvm_abort("");
}

// public native long getLong(Object o, long offset);
static void obj_getLong(struct frame *frame)
{
    jvm_abort("");
}

// public native void putLong(Object o, long offset, long x);
static void obj_putLong(struct frame *frame)
{
    jvm_abort("");
}

// public native float getFloat(Object o, long offset);
static void obj_getFloat(struct frame *frame)
{
    jvm_abort("");
}

// public native void putFloat(Object o, long offset, float x);
static void obj_putFloat(struct frame *frame)
{
    jvm_abort("");
}

// public native double getDouble(Object o, long offset);
static void obj_getDouble(struct frame *frame)
{
    jvm_abort("");
}

// public native void putDouble(Object o, long offset, double x);
static void obj_putDouble(struct frame *frame)
{
    jvm_abort("");
}

// public native Object getObject(Object o, long offset);
static void getObject(struct frame *frame)
{
    jvm_abort("");
}

// public native void putObject(Object o, long offset, Object x);
static void putObject(struct frame *frame)
{
    jvm_abort("");
}

// public native boolean getBooleanVolatile(Object o, long offset);
static void getBooleanVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native byte getByteVolatile(Object o, long offset);
static void getByteVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native char getCharVolatile(Object o, long offset);
static void getCharVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native short getShortVolatile(Object o, long offset);
static void getShortVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native int getIntVolatile(Object o, long offset);
static void getIntVolatile(struct frame *frame)
{
    // todo Volatile
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2);

    jint value;
    if (class_is_array(o->clazz)) {
        value = jarrobj_get(jint, o, offset);  // todo
    } else {
        value = slot_geti(get_instance_field_value_by_id(o, offset));  // todo
    }
    frame_stack_pushi(frame, value);
}

// public native long getLongVolatile(Object o, long offset);
static void getLongVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native float getFloatVolatile(Object o, long offset);
static void getFloatVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native double getDoubleVolatile(Object o, long offset);
static void getDoubleVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putIntVolatile(Object o, long offset, int x);
static void putIntVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putBooleanVolatile(Object o, long offset, boolean x);
static void putBooleanVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putByteVolatile(Object o, long offset, byte x);
static void putByteVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putShortVolatile(Object o, long offset, short x);
static void putShortVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putCharVolatile(Object o, long offset, char x);
static void putCharVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putLongVolatile(Object o, long offset, long x);
static void putLongVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putFloatVolatile(Object o, long offset, float x);
static void putFloatVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native void putDoubleVolatile(Object o, long offset, double x);
static void putDoubleVolatile(struct frame *frame)
{
    jvm_abort("");
}
// -------------------------
// public native Object getObjectVolatile(Object o, long offset);
static void getObjectVolatile(struct frame *frame)
{
    // todo Volatile
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2);

    jref value;
    if (class_is_array(o->clazz)) {
        value = jarrobj_get(jref, o, offset);  // todo
    } else {
        value = slot_getr(get_instance_field_value_by_id(o, offset));  // todo
    }
    frame_stack_pushr(frame, value);
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(struct frame *frame)
{
    jvm_abort("");
}

// public native Object getOrderedObject(Object o, long offset);
static void getOrderedObject(struct frame *frame)
{
    jvm_abort("");
}

// public native void putOrderedObject(Object o, long offset, Object x);
static void putOrderedObject(struct frame *frame)
{
    jvm_abort("");
}

/** Ordered/Lazy version of {@link #putIntVolatile(Object, long, int)}  */
// public native void putOrderedInt(Object o, long offset, int x);
static void putOrderedInt(struct frame *frame)
{
    jvm_abort("");
}

/** Ordered/Lazy version of {@link #putLongVolatile(Object, long, long)} */
// public native void putOrderedLong(Object o, long offset, long x);
static void putOrderedLong(struct frame *frame)
{
    jvm_abort("");
}

/*************************************    unsafe memory    ************************************/
// todo 说明 unsafe memory

/*
 * todo
 * 分配内存方法还有重分配内存方法都是分配的堆外内存，
 * 返回的是一个long类型的地址偏移量。这个偏移量在你的Java程序中每块内存都是唯一的。
 */
// public native long allocateMemory(long bytes);
static void allocateMemory(struct frame *frame)
{
    jlong bytes = frame_locals_getl(frame, 1);
    VM_MALLOCS(u1, bytes, p);
    frame_stack_pushl(frame, (jlong) (intptr_t) p);
}

// public native long reallocateMemory(long address, long bytes);
static void reallocateMemory(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jlong bytes = (jbyte) frame_locals_getl(frame, 3);
    frame_stack_pushl(frame, (jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes)); // 有内存泄漏
}

// public native void freeMemory(long address);
static void freeMemory(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    free((void *) (intptr_t) address);
}

// public native int addressSize();
static void addressSize(struct frame *frame)
{
    frame_stack_pushi(frame, sizeof(jlong)); // todo
}

// public native void putByte(long address, byte x);
static void putByte(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jbyte x = (jbyte) frame_locals_geti(frame, 3);
    *(jbyte *) (intptr_t) address = BIGENDIAN(x); // todo java按大端存储？？？？？？？
}

// public native byte getByte(long address);
static void getByte(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushi(frame, *(jbyte *) (intptr_t) address);
}

// public native void putChar(long address, char x);
static void putChar(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jchar x = (jchar) frame_locals_geti(frame, 3);
    *(jchar *) (intptr_t) address = BIGENDIAN(x);
}

// public native char getChar(long address);
static void getChar(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushi(frame, *(jchar *) (intptr_t) address);
}

// public native void putShort(long address, short x);
static void putShort(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jshort x = (jshort) frame_locals_geti(frame, 3);
    *(jshort *) (intptr_t) address = BIGENDIAN(x);
}

// public native short getShort(long address);
static void getShort(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushi(frame, *(jshort *) (intptr_t) address);
}

// public native void putInt(long address, int x);
static void putInt(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jint x = frame_locals_geti(frame, 3);
    *(jint *) (intptr_t) address = BIGENDIAN(x);
}

// public native int getInt(long address);
static void getInt(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushi(frame, *(jint *) (intptr_t) address);
}

// public native void putLong(long address, long x);
static void putLong(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jlong x = frame_locals_getl(frame, 3);
    *(jlong *) (intptr_t) address = BIGENDIAN(x);
}

// public native long getLong(long address);
static void getLong(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushl(frame, *(jlong *) (intptr_t) address);
}

// public native void putFloat(long address, float x);
static void putFloat(struct frame *frame)
{
    jlong address = frame_locals_getl(frame,+ 1);
    jfloat x = frame_locals_getf(frame, 3);
    *(jfloat *) (intptr_t) address = BIGENDIAN(x);
}

// public native float getFloat(long address);
static void getFloat(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushf(frame, *(jfloat *) (intptr_t) address);
}

// public native void putDouble(long address, double x);
static void putDouble(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    jdouble x = frame_locals_getd(frame, 3);
    *(jdouble *) (intptr_t) address = BIGENDIAN(x);
}

// public native double getDouble(long address);
static void getDouble(struct frame *frame)
{
    jlong address = frame_locals_getl(frame, 1);
    frame_stack_pushd(frame, *(jdouble *) (intptr_t) address);
}

// public native void putAddress(long address, long x);
static void putAddress(struct frame *frame)
{
    putLong(frame);
}

// public native long getAddress(long address);
static void getAddress(struct frame *frame)
{
    getLong(frame);
}

/**
  499        * Sets all bytes in a given block of memory to a fixed value
  500        * (usually zero).
  501        *
  502        * <p>This method determines a block's base address by means of two parameters,
  503        * and so it provides (in effect) a <em>double-register</em> addressing mode,
  504        * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
  505        * the offset supplies an absolute base address.
  506        *
  507        * <p>The stores are in coherent (atomic) units of a size determined
  508        * by the address and length parameters.  If the effective address and
  509        * length are all even modulo 8, the stores take place in 'long' units.
  510        * If the effective address and length are (resp.) even modulo 4 or 2,
  511        * the stores take place in units of 'int' or 'short'.
  512        *
  513        * @since 1.7
  514        */
// public native void setMemory(Object o, long offset, long bytes, byte value);
static void setMemory(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  529        * Sets all bytes in a given block of memory to a copy of another
  530        * block.
  531        *
  532        * <p>This method determines each block's base address by means of two parameters,
  533        * and so it provides (in effect) a <em>double-register</em> addressing mode,
  534        * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
  535        * the offset supplies an absolute base address.
  536        *
  537        * <p>The transfers are in coherent (atomic) units of a size determined
  538        * by the address and length parameters.  If the effective addresses and
  539        * length are all even modulo 8, the transfer takes place in 'long' units.
  540        * If the effective addresses and length are (resp.) even modulo 4 or 2,
  541        * the transfer takes place in units of 'int' or 'short'.
  542        *
  543        * @since 1.7
  544        */
// public native void copyMemory(Object srcBase, long srcOffset, Object destBase, long destOffset, long bytes);
static void copyMemory(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  988        * Gets the load average in the system run queue assigned
  989        * to the available processors averaged over various periods of time.
  990        * This method retrieves the given <tt>nelem</tt> samples and
  991        * assigns to the elements of the given <tt>loadavg</tt> array.
  992        * The system imposes a maximum of 3 samples, representing
  993        * averages over the last 1,  5,  and  15 minutes, respectively.
  994        *
  995        * @params loadavg an array of double of size nelems
  996        * @params nelems the number of samples to be retrieved and
  997        *         must be 1 to 3.
  998        *
  999        * @return the number of samples actually retrieved; or -1
 1000        *         if the load average is unobtainable.
 1001        */
// public native int getLoadAverage(double[] loadavg, int nelems);
static void getLoadAverage(struct frame *frame)
{
    jvm_abort(""); // todo
}

static void shouldBeInitialized(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  796        * Report the size in bytes of a native memory page (whatever that is).
  797        * This value will always be a power of two.
  798        */
// public native int pageSize();
static void pageSize(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  815        * Define a class but do not make it known to the class loader or system dictionary.
  816        * <p>
  817        * For each CP entry, the corresponding CP patch must either be null or have
  818        * the a format that matches its tag:
  819        * <ul>
  820        * <li>Integer, Long, Float, Double: the corresponding wrapper object type from java.lang
  821        * <li>Utf8: a string (must have suitable syntax if used as signature or name)
  822        * <li>Class: any java.lang.Class object
  823        * <li>String: any object (not just a java.lang.String)
  824        * <li>InterfaceMethodRef: (NYI) a method handle to invoke on that call site's arguments
  825        * </ul>
  826        * @params hostClass context for linkage, access control, protection domain, and class loader
  827        * @params data      bytes of a class file
  828        * @params cpPatches where non-null entries exist, they replace corresponding CP entries in data
  829        */
// public native Class defineAnonymousClass(Class hostClass, byte[] data, Object[] cpPatches);
static void defineAnonymousClass(struct frame *frame)
{
    jvm_abort(""); // todo
}

/** Lock the object.  It must get unlocked via {@link #monitorExit}. */
//  public native void monitorEnter(Object o);
static void monitorEnter(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  842        * Unlock the object.  It must have been locked via {@link
  843        * #monitorEnter}.
  844        */
// public native void monitorExit(Object o);
static void monitorExit(struct frame *frame)
{
    jvm_abort(""); // todo
}

/**
  848        * Tries to lock the object.  Returns true or false to indicate
  849        * whether the lock succeeded.  If it did, the object must be
  850        * unlocked via {@link #monitorExit}.
  851        */
// public native boolean tryMonitorEnter(Object o);
static void tryMonitorEnter(struct frame *frame)
{
    jvm_abort(""); // todo
}

/** Throw the exception without telling the verifier. */
// public native void throwException(Throwable ee);
static void throwException(struct frame *frame)
{
    jvm_abort(""); // todo
}

static void loadFence(struct frame *frame)
{
    jvm_abort(""); // todo
}

static void storeFence(struct frame *frame)
{
    jvm_abort(""); // todo
}

static void fullFence(struct frame *frame)
{
    jvm_abort(""); // todo
}

void sun_misc_Unsafe_registerNatives()
{
    register_native_method("sun/misc/Unsafe~registerNatives~()V", registerNatives);

    register_native_method("sun/misc/Unsafe~park~(ZJ)V", park);
    register_native_method("sun/misc/Unsafe~unpark~(Ljava/lang/Object;)V", unpark);

    // compare and swap
    register_native_method("sun/misc/Unsafe~compareAndSwapInt~(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    register_native_method("sun/misc/Unsafe~compareAndSwapLong~(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    register_native_method("sun/misc/Unsafe~"
                                   "compareAndSwapObject~"
                                   "(Ljava/lang/Object;JLjava/lang/Object;Ljava/lang/Object;)Z",
                           compareAndSwapObject);

    // class
    register_native_method("sun/misc/Unsafe~allocateInstance~(Ljava/lang/Class;)Ljava/lang/Object;", allocateInstance);
    register_native_method("sun/misc/Unsafe~defineClass~"
                    "(Ljava/lang/String;[BIILjava/lang/ClassLoader;Ljava/security/ProtectionDomain;)Ljava/lang/Class;",
                           defineClass);
    register_native_method("sun/misc/Unsafe~ensureClassInitialized~(Ljava/lang/Class;)V", ensureClassInitialized);
    register_native_method("sun/misc/Unsafe~staticFieldOffset~(Ljava/lang/reflect/Field;)J", staticFieldOffset);
    register_native_method("sun/misc/Unsafe~staticFieldBase~(Ljava/lang/reflect/Field;)Ljava/lang/Object;", staticFieldBase);

    // object
    register_native_method("sun/misc/Unsafe~arrayBaseOffset~(Ljava/lang/Class;)I", arrayBaseOffset);
    register_native_method("sun/misc/Unsafe~arrayIndexScale~(Ljava/lang/Class;)I", arrayIndexScale);
    register_native_method("sun/misc/Unsafe~objectFieldOffset~(Ljava/lang/reflect/Field;)J", objectFieldOffset);
    
    register_native_method("sun/misc/Unsafe~getBoolean~(Ljava/lang/Object;J)Z", getBoolean);
    register_native_method("sun/misc/Unsafe~putBoolean~(Ljava/lang/Object;JZ)V", putBoolean);
    register_native_method("sun/misc/Unsafe~getByte~(Ljava/lang/Object;J)B", obj_getByte);
    register_native_method("sun/misc/Unsafe~putByte~(Ljava/lang/Object;JB)V", obj_putByte);
    register_native_method("sun/misc/Unsafe~getChar~(Ljava/lang/Object;J)C", obj_getChar);
    register_native_method("sun/misc/Unsafe~putChar~(Ljava/lang/Object;JC)V", obj_putChar);
    register_native_method("sun/misc/Unsafe~getShort~(Ljava/lang/Object;J)S", obj_getShort);
    register_native_method("sun/misc/Unsafe~putShort~(Ljava/lang/Object;JS)V", obj_putShort);
    register_native_method("sun/misc/Unsafe~getInt~(Ljava/lang/Object;J)I", obj_getInt);
    register_native_method("sun/misc/Unsafe~putInt~(Ljava/lang/Object;JI)V", obj_putInt);
    register_native_method("sun/misc/Unsafe~getLong~(Ljava/lang/Object;J)J", obj_getLong);
    register_native_method("sun/misc/Unsafe~putLong~(Ljava/lang/Object;JJ)V", obj_putLong);
    register_native_method("sun/misc/Unsafe~getFloat~(Ljava/lang/Object;J)F", obj_getFloat);
    register_native_method("sun/misc/Unsafe~putFloat~(Ljava/lang/Object;JF)V", obj_putFloat);
    register_native_method("sun/misc/Unsafe~getDouble~(Ljava/lang/Object;J)D", obj_getDouble);
    register_native_method("sun/misc/Unsafe~putDouble~(Ljava/lang/Object;JD)V", obj_putDouble);
    register_native_method("sun/misc/Unsafe~getObject~(Ljava/lang/Object;J)Ljava/lang/Object;", getObject);
    register_native_method("sun/misc/Unsafe~putObject~(Ljava/lang/Object;JLjava/lang/Object;)V", putObject);
    register_native_method("sun/misc/Unsafe~getObjectVolatile~(Ljava/lang/Object;J)Ljava/lang/Object;", getObjectVolatile);
    register_native_method("sun/misc/Unsafe~putObjectVolatile~(Ljava/lang/Object;JLjava/lang/Object;)V", putObjectVolatile);
    register_native_method("sun/misc/Unsafe~getOrderedObject~(Ljava/lang/Object;J)Ljava/lang/Object;", getOrderedObject);
    register_native_method("sun/misc/Unsafe~putOrderedObject~(Ljava/lang/Object;JLjava/lang/Object;)V", putOrderedObject);
    register_native_method("sun/misc/Unsafe~putOrderedInt~(Ljava/lang/Object;JI)V", putOrderedInt);
    register_native_method("sun/misc/Unsafe~putOrderedLong~(Ljava/lang/Object;JJ)V", putOrderedLong);

    register_native_method("sun/misc/Unsafe~putIntVolatile~(Ljava/lang/Object;JI)V", putIntVolatile);
    register_native_method("sun/misc/Unsafe~putBooleanVolatile~(Ljava/lang/Object;JZ)V", putBooleanVolatile);
    register_native_method("sun/misc/Unsafe~putByteVolatile~(Ljava/lang/Object;JB)V", putByteVolatile);
    register_native_method("sun/misc/Unsafe~putShortVolatile~(Ljava/lang/Object;JS)V", putShortVolatile);
    register_native_method("sun/misc/Unsafe~putCharVolatile~(Ljava/lang/Object;JC)V", putCharVolatile);
    register_native_method("sun/misc/Unsafe~putLongVolatile~(Ljava/lang/Object;JJ)V", putLongVolatile);
    register_native_method("sun/misc/Unsafe~putFloatVolatile~(Ljava/lang/Object;JF)V", putFloatVolatile);
    register_native_method("sun/misc/Unsafe~putDoubleVolatile~(Ljava/lang/Object;JD)V", putDoubleVolatile);

    register_native_method("sun/misc/Unsafe~getIntVolatile~(Ljava/lang/Object;J)I", getIntVolatile);
    register_native_method("sun/misc/Unsafe~getBooleanVolatile~(Ljava/lang/Object;J)Z", getBooleanVolatile);
    register_native_method("sun/misc/Unsafe~getByteVolatile~(Ljava/lang/Object;J)B", getByteVolatile);
    register_native_method("sun/misc/Unsafe~getShortVolatile~(Ljava/lang/Object;J)S", getShortVolatile);
    register_native_method("sun/misc/Unsafe~getCharVolatile~(Ljava/lang/Object;J)C", getCharVolatile);
    register_native_method("sun/misc/Unsafe~getLongVolatile~(Ljava/lang/Object;J)J", getLongVolatile);
    register_native_method("sun/misc/Unsafe~getFloatVolatile~(Ljava/lang/Object;J)F", getFloatVolatile);
    register_native_method("sun/misc/Unsafe~getDoubleVolatile~(Ljava/lang/Object;J)D", getDoubleVolatile);

    // unsafe memory
    register_native_method("sun/misc/Unsafe~allocateMemory~(J)J", allocateMemory);
    register_native_method("sun/misc/Unsafe~reallocateMemory~(JJ)J", reallocateMemory);
    register_native_method("sun/misc/Unsafe~setMemory~(Ljava/lang/Object;JJB)V", setMemory);
    register_native_method("sun/misc/Unsafe~copyMemory~(Ljava/lang/Object;JLjava/lang/Object;JJ)V", copyMemory);
    register_native_method("sun/misc/Unsafe~freeMemory~(J)V", freeMemory);
    register_native_method("sun/misc/Unsafe~addressSize~()I", addressSize);
    register_native_method("sun/misc/Unsafe~putAddress~(JJ)V", putAddress);
    register_native_method("sun/misc/Unsafe~getAddress~(J)J", getAddress);
    register_native_method("sun/misc/Unsafe~putByte~(JB)V", putByte);
    register_native_method("sun/misc/Unsafe~getByte~(J)B", getByte);
    register_native_method("sun/misc/Unsafe~putShort~(JS)V", putShort);
    register_native_method("sun/misc/Unsafe~getShort~(J)S", getShort);
    register_native_method("sun/misc/Unsafe~putChar~(JC)V", putChar);
    register_native_method("sun/misc/Unsafe~getChar~(J)C", getChar);
    register_native_method("sun/misc/Unsafe~putInt~(JI)V", putInt);
    register_native_method("sun/misc/Unsafe~getInt~(J)I", getInt);
    register_native_method("sun/misc/Unsafe~putLong~(JJ)V", putLong);
    register_native_method("sun/misc/Unsafe~getLong~(J)J", getLong);
    register_native_method("sun/misc/Unsafe~putFloat~(JF)V", putFloat);
    register_native_method("sun/misc/Unsafe~getFloat~(J)F", getFloat);
    register_native_method("sun/misc/Unsafe~putDouble~(JD)V", putDouble);
    register_native_method("sun/misc/Unsafe~getDouble~(J)D", getDouble);

    register_native_method("sun/misc/Unsafe~shouldBeInitialized~(Ljava/lang/Class;)Z", shouldBeInitialized);
    register_native_method("sun/misc/Unsafe~getLoadAverage~([DI)I", getLoadAverage);
    register_native_method("sun/misc/Unsafe~pageSize~()I", pageSize);
    register_native_method(
            "sun/misc/Unsafe~defineAnonymousClass~(Ljava/lang/Class;[B[Ljava/lang/Object;)Ljava/lang/Class;",
            defineAnonymousClass);
    register_native_method("sun/misc/Unsafe~monitorEnter~(Ljava/lang/Object;)V", monitorEnter);
    register_native_method("sun/misc/Unsafe~monitorExit~(Ljava/lang/Object;)V", monitorExit);
    register_native_method("sun/misc/Unsafe~tryMonitorEnter~(Ljava/lang/Object;)Z", tryMonitorEnter);
    register_native_method("sun/misc/Unsafe~throwException~(Ljava/lang/Throwable;)V", throwException);

    register_native_method("sun/misc/Unsafe~loadFence~()V", loadFence);
    register_native_method("sun/misc/Unsafe~storeFence~()V", storeFence);
    register_native_method("sun/misc/Unsafe~fullFence~()V", fullFence);
}
