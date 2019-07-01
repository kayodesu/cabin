/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/heap/Object.h"
#include "../../../util/endianness.h"
#include "../../../rtda/heap/ArrayObject.h"
#include "../../../symbol.h"

/* todo
http://www.docjar.com/docs/api/sun/misc/Unsafe.html#park%28boolean,%20long%29
Block current Thread, returning when a balancing
unpark occurs, or a balancing unpark has
already occurred, or the Thread is interrupted, or, if not
absolute and time is not zero, the given time nanoseconds have
elapsed, or if absolute, the given deadline in milliseconds
since Epoch has passed, or spuriously (i.e., returning for no
"reason"). Note: This operation is in the Unsafe class only
because unpark is, so it would be strange to place it
elsewhere.
*/

// public native void park(boolean isAbsolute, long time);
static void park(Frame *frame)
{
    jvm_abort("park");
}

//  public native void unpark(Object thread);
static void unpark(Frame *frame)
{
    jvm_abort("unpark");
}

/*************************************    compare and swap    ************************************/

/*
 * 第一个参数为需要改变的对象，
 * 第二个为偏移量(参见函数 objectFieldOffset)，
 * 第三个参数为期待的值，
 * 第四个为更新后的值。
 *
 * 整个方法的作用即为若调用该方法时，value的值与expect这个值相等，那么则将value修改为update这个值，并返回一个true，
 * 如果调用该方法时，value的值与expect这个值不相等，那么不做任何操作，并范围一个false。
 *
 * public final native boolean compareAndSwapInt(Object o, long offset, int expected, int x);
 */
static void compareAndSwapInt(Frame *frame)
{
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2); // long 占两个Slot
    jint expected = frame->getLocalAsInt(4);
    jint x = frame->getLocalAsInt(5);

    jint old;
    if (o->isArray()) {
        old = ((ArrayObject *) o)->get<jint>(offset);//arrobj_get(jint, o, offset);
    } else {
        old = o->getInstFieldValue<jint>(offset);//ISLOT(o->getInstFieldValue(offset));
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, x);
    frame_stack_pushi(frame, b ? 1 : 0);
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(Frame *frame)
{
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2); // long 占两个Slot
    jlong expected = frame->getLocalAsLong(4);
    jlong x = frame->getLocalAsLong(6);

    jlong old;
    if (o->isArray()) {
//        old = arrobj_get(jlong, o, offset);
        ArrayObject *ao = dynamic_cast<ArrayObject *>(o);  // todo
        old = ao->get<jlong>(offset);//arrobj_get(jint, o, offset);
    } else {
//        old = LSLOT(get_instance_field_value_by_id(o, offset));
        old = o->getInstFieldValue<jlong>(offset);
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, x);
    frame_stack_pushi(frame, b ? 1 : 0);
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(Frame *frame)
{
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2); // long 占两个Slot
    jref expected = frame->getLocalAsRef(4);
    jref x = frame->getLocalAsRef(5);

    jref old;
    if (o->isArray()) {
        ArrayObject *ao = dynamic_cast<ArrayObject *>(o);  // todo
        old = ao->get<jref>(offset);
//        old = arrobj_get(jref, o, offset);
    } else {
        old = o->getInstFieldValue<jref>(offset);
    }

    bool b = __sync_bool_compare_and_swap(&old, expected, x);
    frame_stack_pushi(frame, b ? 1 : 0);
}

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static void allocateInstance(Frame *frame)
{
    jvm_abort("allocateInstance");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static void defineClass(Frame *frame)
{
    jvm_abort("defineClass");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(Frame *frame)
{
    jvm_abort("ensureClassInitialized");
}

// public native Object staticFieldBase(Field f);
// public native long staticFieldOffset(Field f);
static void staticFieldOffset(Frame *frame)
{
    jvm_abort("staticFieldOffset");
}

static void staticFieldBase(Frame *frame)
{
    jvm_abort("staticFieldBase");
}
/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static void arrayBaseOffset(Frame *frame)
{
    frame_stack_pushi(frame, 0);//frame->operandStack.push((jint)0);  // todo
}

// public native int arrayIndexScale(Class<?> type)
static void arrayIndexScale(Frame *frame)
{
    frame_stack_pushi(frame, 1);//frame->operandStack.push((jint)1);  // todo
}

// public native long objectFieldOffset(Field field)
static void objectFieldOffset(Frame *frame)
{
    /*
     * 	vars := Frame.LocalVars()
	jField := vars.GetRef(1)

	offset := jField.GetFieldValue("slot~I").(int32)

	stack := Frame.OperandStack()
	stack.PushLong(int64(offset))
     */

//    JObject *jlrFieldObj = frame->getLocalVar(1).getRef();
//    jint offset = jlrFieldObj->getFieldValue("slot", "I").getInt();
//    frame->operandStack.push((jlong)offset);

    jref field_obj = frame->getLocalAsRef(1);
    jint offset = field_obj->getInstFieldValue<jint>(S(slot), S(I)); // todo "slot", "I" 什么东西
//    printvm("-------   %s, %d\n", jobject_to_string(field_obj), offset);
    frame_stack_pushl(frame, offset);
}

// public native boolean getBoolean(Object o, long offset);
static void getBoolean(Frame *frame)
{
    jvm_abort("getBoolean");
}

// public native void putBoolean(Object o, long offset, boolean x);
static void putBoolean(Frame *frame)
{
    jvm_abort("putBoolean");
}

// public native byte getByte(Object o, long offset);
static void obj_getByte(Frame *frame)
{
    jvm_abort("obj_getByte");
}

// public native void putByte(Object o, long offset, byte x);
static void obj_putByte(Frame *frame)
{
    jvm_abort("obj_putByte");
}

// public native char getChar(Object o, long offset);
static void obj_getChar(Frame *frame)
{
    jvm_abort("obj_getChar");
}

// public native void putChar(Object o, long offset, char x);
static void obj_putChar(Frame *frame)
{
    jvm_abort("obj_putChar");
}

// public native short getShort(Object o, long offset);
static void obj_getShort(Frame *frame)
{
    jvm_abort("obj_getShort");
}

// public native void putShort(Object o, long offset, short x);
static void obj_putShort(Frame *frame)
{
    jvm_abort("obj_putShort");
}

// public native int getInt(Object o, long offset);
static void obj_getInt(Frame *frame)
{
    jvm_abort("obj_getInt");
}

// public native void putInt(Object o, long offset, int x);
static void obj_putInt(Frame *frame)
{
    jvm_abort("obj_putInt");
}

// public native long getLong(Object o, long offset);
static void obj_getLong(Frame *frame)
{
    jvm_abort("obj_getLong");
}

// public native void putLong(Object o, long offset, long x);
static void obj_putLong(Frame *frame)
{
    jvm_abort("obj_putLong");
}

// public native float getFloat(Object o, long offset);
static void obj_getFloat(Frame *frame)
{
    jvm_abort("obj_getFloat");
}

// public native void putFloat(Object o, long offset, float x);
static void obj_putFloat(Frame *frame)
{
    jvm_abort("obj_putFloat");
}

// public native double getDouble(Object o, long offset);
static void obj_getDouble(Frame *frame)
{
    jvm_abort("obj_getDouble");
}

// public native void putDouble(Object o, long offset, double x);
static void obj_putDouble(Frame *frame)
{
    jvm_abort("obj_putDouble");
}

// public native Object getObject(Object o, long offset);
static void getObject(Frame *frame)
{
    jvm_abort("getObject");
}

// public native void putObject(Object o, long offset, Object x);
static void putObject(Frame *frame)
{
    jvm_abort("putObject");
}

// public native boolean getBooleanVolatile(Object o, long offset);
static void getBooleanVolatile(Frame *frame)
{
    jvm_abort("getBooleanVolatile");
}

// public native byte getByteVolatile(Object o, long offset);
static void getByteVolatile(Frame *frame)
{
    jvm_abort("getByteVolatile");
}

// public native char getCharVolatile(Object o, long offset);
static void getCharVolatile(Frame *frame)
{
    jvm_abort("getCharVolatile");
}

// public native short getShortVolatile(Object o, long offset);
static void getShortVolatile(Frame *frame)
{
    jvm_abort("getShortVolatile");
}

// public native int getIntVolatile(Object o, long offset);
static void getIntVolatile(Frame *frame)
{
    // todo Volatile
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2);

    jint value;
    if (o->isArray()) {
        ArrayObject *ao = dynamic_cast<ArrayObject *>(o);  // todo
        value = ao->get<jint>(offset);
//        value = arrobj_get(jint, o, offset);  // todo
    } else {
        value = o->getInstFieldValue<jint>(offset);  // todo
    }
    frame_stack_pushi(frame, value);
}

// public native long getLongVolatile(Object o, long offset);
static void getLongVolatile(Frame *frame)
{
    jvm_abort("getLongVolatile");
}

// public native float getFloatVolatile(Object o, long offset);
static void getFloatVolatile(Frame *frame)
{
    jvm_abort("getFloatVolatile");
}

// public native double getDoubleVolatile(Object o, long offset);
static void getDoubleVolatile(Frame *frame)
{
    jvm_abort("getDoubleVolatile");
}

// public native void putIntVolatile(Object o, long offset, int x);
static void putIntVolatile(Frame *frame)
{
    jvm_abort("putIntVolatile");
}

// public native void putBooleanVolatile(Object o, long offset, boolean x);
static void putBooleanVolatile(Frame *frame)
{
    jvm_abort("putBooleanVolatile");
}

// public native void putByteVolatile(Object o, long offset, byte x);
static void putByteVolatile(Frame *frame)
{
    jvm_abort("putByteVolatile");
}

// public native void putShortVolatile(Object o, long offset, short x);
static void putShortVolatile(Frame *frame)
{
    jvm_abort("putShortVolatile");
}

// public native void putCharVolatile(Object o, long offset, char x);
static void putCharVolatile(Frame *frame)
{
    jvm_abort("putCharVolatile");
}

// public native void putLongVolatile(Object o, long offset, long x);
static void putLongVolatile(Frame *frame)
{
    jvm_abort("putLongVolatile");
}

// public native void putFloatVolatile(Object o, long offset, float x);
static void putFloatVolatile(Frame *frame)
{
    jvm_abort("putFloatVolatile");
}

// public native void putDoubleVolatile(Object o, long offset, double x);
static void putDoubleVolatile(Frame *frame)
{
    jvm_abort("putDoubleVolatile");
}
// -------------------------
// public native Object getObjectVolatile(Object o, long offset);
static void getObjectVolatile(Frame *frame)
{
    // todo Volatile
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2);

    jref value;
    if (o->isArray()) {
        ArrayObject *ao = dynamic_cast<ArrayObject *>(o);  // todo
        value = ao->get<jref>(offset);
    } else {
        value = o->getInstFieldValue<jref>(offset);  // todo
    }
    frame_stack_pushr(frame, value);
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(Frame *frame)
{
    jvm_abort("putObjectVolatile");
}

// public native Object getOrderedObject(Object o, long offset);
static void getOrderedObject(Frame *frame)
{
    jvm_abort("getOrderedObject");
}

// public native void putOrderedObject(Object o, long offset, Object x);
static void putOrderedObject(Frame *frame)
{
    jvm_abort("putOrderedObject");
}

/** Ordered/Lazy version of {@link #putIntVolatile(Object, long, int)}  */
// public native void putOrderedInt(Object o, long offset, int x);
static void putOrderedInt(Frame *frame)
{
    jvm_abort("putOrderedInt");
}

/** Ordered/Lazy version of {@link #putLongVolatile(Object, long, long)} */
// public native void putOrderedLong(Object o, long offset, long x);
static void putOrderedLong(Frame *frame)
{
    jvm_abort("putOrderedLong");
}

/*************************************    unsafe memory    ************************************/
// todo 说明 unsafe memory

/*
 * todo
 * 分配内存方法还有重分配内存方法都是分配的堆外内存，
 * 返回的是一个long类型的地址偏移量。这个偏移量在你的Java程序中每块内存都是唯一的。
 */
// public native long allocateMemory(long bytes);
static void allocateMemory(Frame *frame)
{
    jlong bytes = frame->getLocalAsLong(1);
    u1 *p = (u1 *) vm_malloc(sizeof(char)*bytes); // todo
    frame_stack_pushl(frame, (jlong) (intptr_t) p);
}

// public native long reallocateMemory(long address, long bytes);
static void reallocateMemory(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jlong bytes = frame->getLocalAsLong(3);
    frame_stack_pushl(frame, (jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes)); // 有内存泄漏
}

// public native void freeMemory(long address);
static void freeMemory(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    free((void *) (intptr_t) address);
}

// public native int addressSize();
static void addressSize(Frame *frame)
{
    frame_stack_pushi(frame, sizeof(jlong)); // todo
}

// public native void putByte(long address, byte x);
static void putByte(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jbyte x = (jbyte) frame->getLocalAsInt(3);
    *(jbyte *) (intptr_t) address = changeToBigEndian(x); // todo java按大端存储？？？？？？？
}

// public native byte getByte(long address);
static void getByte(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushi(frame, *(jbyte *) (intptr_t) address);
}

// public native void putChar(long address, char x);
static void putChar(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jchar x = (jchar) frame->getLocalAsInt(3);
    *(jchar *) (intptr_t) address = changeToBigEndian(x);
}

// public native char getChar(long address);
static void getChar(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushi(frame, *(jchar *) (intptr_t) address);
}

// public native void putShort(long address, short x);
static void putShort(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jshort x = frame->getLocalAsShort(3);
    *(jshort *) (intptr_t) address = changeToBigEndian(x);
}

// public native short getShort(long address);
static void getShort(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushi(frame, *(jshort *) (intptr_t) address);
}

// public native void putInt(long address, int x);
static void putInt(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jint x = frame->getLocalAsInt(3);
    *(jint *) (intptr_t) address = changeToBigEndian(x);
}

// public native int getInt(long address);
static void getInt(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushi(frame, *(jint *) (intptr_t) address);
}

// public native void putLong(long address, long x);
static void putLong(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jlong x = frame->getLocalAsLong(3);
    *(jlong *) (intptr_t) address = changeToBigEndian(x);
}

// public native long getLong(long address);
static void getLong(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushl(frame, *(jlong *) (intptr_t) address);
}

// public native void putFloat(long address, float x);
static void putFloat(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jfloat x = frame->getLocalAsFloat(3);
    *(jfloat *) (intptr_t) address = changeToBigEndian(x);
}

// public native float getFloat(long address);
static void getFloat(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushf(frame, *(jfloat *) (intptr_t) address);
}

// public native void putDouble(long address, double x);
static void putDouble(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jdouble x = frame->getLocalAsDouble(3);
    *(jdouble *) (intptr_t) address = changeToBigEndian(x);
}

// public native double getDouble(long address);
static void getDouble(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    frame_stack_pushd(frame, *(jdouble *) (intptr_t) address);
}

// public native void putAddress(long address, long x);
static void putAddress(Frame *frame)
{
    putLong(frame);
}

// public native long getAddress(long address);
static void getAddress(Frame *frame)
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
static void setMemory(Frame *frame)
{
    jvm_abort("setMemory"); // todo
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
static void copyMemory(Frame *frame)
{
    jvm_abort("copyMemory"); // todo
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
static void getLoadAverage(Frame *frame)
{
    jvm_abort("getLoadAverage"); // todo
}

static void shouldBeInitialized(Frame *frame)
{
    jvm_abort("shouldBeInitialized"); // todo
}

/**
  796        * Report the size in bytes of a native memory page (whatever that is).
  797        * This value will always be a power of two.
  798        */
// public native int pageSize();
static void pageSize(Frame *frame)
{
    jvm_abort("pageSize"); // todo
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
static void defineAnonymousClass(Frame *frame)
{
    jvm_abort("defineAnonymousClass"); // todo
}

/** Lock the object.  It must get unlocked via {@link #monitorExit}. */
//  public native void monitorEnter(Object o);
static void monitorEnter(Frame *frame)
{
    jvm_abort("monitorEnter"); // todo
}

/**
  842        * Unlock the object.  It must have been locked via {@link
  843        * #monitorEnter}.
  844        */
// public native void monitorExit(Object o);
static void monitorExit(Frame *frame)
{
    jvm_abort("monitorExit"); // todo
}

/**
  848        * Tries to lock the object.  Returns true or false to indicate
  849        * whether the lock succeeded.  If it did, the object must be
  850        * unlocked via {@link #monitorExit}.
  851        */
// public native boolean tryMonitorEnter(Object o);
static void tryMonitorEnter(Frame *frame)
{
    jvm_abort("tryMonitorEnter"); // todo
}

/** Throw the exception without telling the verifier. */
// public native void throwException(Throwable ee);
static void throwException(Frame *frame)
{
    jvm_abort("throwException"); // todo
}

static void loadFence(Frame *frame)
{
    jvm_abort("loadFence"); // todo
}

static void storeFence(Frame *frame)
{
    jvm_abort("storeFence"); // todo
}

static void fullFence(Frame *frame)
{
    jvm_abort("fullFence"); // todo
}

void sun_misc_Unsafe_registerNatives()
{
#define C "sun/misc/Unsafe",
#define LCLD "Ljava/lang/ClassLoader;"
    register_native_method(C"park", "(ZJ)V", park);
    register_native_method(C"unpark", "(Ljava/lang/Object;)V", unpark);

    // compare and swap
    register_native_method(C"compareAndSwapInt", "(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    register_native_method(C"compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    register_native_method(C"compareAndSwapObject", "(" LOBJ "J" LOBJ LOBJ")Z", compareAndSwapObject);

    // class
    register_native_method(C"allocateInstance", "(Ljava/lang/Class;)Ljava/lang/Object;", allocateInstance);
    register_native_method(C"defineClass", "(" LSTR "[BII" LCLD "Ljava/security/ProtectionDomain;)" LCLS,defineClass);
    register_native_method(C"ensureClassInitialized", "(Ljava/lang/Class;)V", ensureClassInitialized);
    register_native_method(C"staticFieldOffset", "(Ljava/lang/reflect/Field;)J", staticFieldOffset);
    register_native_method(C"staticFieldBase", "(Ljava/lang/reflect/Field;)" LOBJ, staticFieldBase);

    // Object
    register_native_method(C"arrayBaseOffset", "(Ljava/lang/Class;)I", arrayBaseOffset);
    register_native_method(C"arrayIndexScale", "(Ljava/lang/Class;)I", arrayIndexScale);
    register_native_method(C"objectFieldOffset", "(Ljava/lang/reflect/Field;)J", objectFieldOffset);
    
    register_native_method(C"getBoolean", "(Ljava/lang/Object;J)Z", getBoolean);
    register_native_method(C"putBoolean", "(Ljava/lang/Object;JZ)V", putBoolean);
    register_native_method(C"getByte", "(Ljava/lang/Object;J)B", obj_getByte);
    register_native_method(C"putByte", "(Ljava/lang/Object;JB)V", obj_putByte);
    register_native_method(C"getChar", "(Ljava/lang/Object;J)C", obj_getChar);
    register_native_method(C"putChar", "(Ljava/lang/Object;JC)V", obj_putChar);
    register_native_method(C"getShort", "(Ljava/lang/Object;J)S", obj_getShort);
    register_native_method(C"putShort", "(Ljava/lang/Object;JS)V", obj_putShort);
    register_native_method(C"getInt", "(Ljava/lang/Object;J)I", obj_getInt);
    register_native_method(C"putInt", "(Ljava/lang/Object;JI)V", obj_putInt);
    register_native_method(C"getLong", "(Ljava/lang/Object;J)J", obj_getLong);
    register_native_method(C"putLong", "(Ljava/lang/Object;JJ)V", obj_putLong);
    register_native_method(C"getFloat", "(Ljava/lang/Object;J)F", obj_getFloat);
    register_native_method(C"putFloat", "(Ljava/lang/Object;JF)V", obj_putFloat);
    register_native_method(C"getDouble", "(Ljava/lang/Object;J)D", obj_getDouble);
    register_native_method(C"putDouble", "(Ljava/lang/Object;JD)V", obj_putDouble);
    register_native_method(C"getObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObject);
    register_native_method(C"putObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObject);
    register_native_method(C"getObjectVolatile", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObjectVolatile);
    register_native_method(C"putObjectVolatile", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObjectVolatile);
    register_native_method(C"getOrderedObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getOrderedObject);
    register_native_method(C"putOrderedObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putOrderedObject);
    register_native_method(C"putOrderedInt", "(Ljava/lang/Object;JI)V", putOrderedInt);
    register_native_method(C"putOrderedLong", "(Ljava/lang/Object;JJ)V", putOrderedLong);

    register_native_method(C"putIntVolatile", "(Ljava/lang/Object;JI)V", putIntVolatile);
    register_native_method(C"putBooleanVolatile", "(Ljava/lang/Object;JZ)V", putBooleanVolatile);
    register_native_method(C"putByteVolatile", "(Ljava/lang/Object;JB)V", putByteVolatile);
    register_native_method(C"putShortVolatile", "(Ljava/lang/Object;JS)V", putShortVolatile);
    register_native_method(C"putCharVolatile", "(Ljava/lang/Object;JC)V", putCharVolatile);
    register_native_method(C"putLongVolatile", "(Ljava/lang/Object;JJ)V", putLongVolatile);
    register_native_method(C"putFloatVolatile", "(Ljava/lang/Object;JF)V", putFloatVolatile);
    register_native_method(C"putDoubleVolatile", "(Ljava/lang/Object;JD)V", putDoubleVolatile);

    register_native_method(C"getIntVolatile", "(Ljava/lang/Object;J)I", getIntVolatile);
    register_native_method(C"getBooleanVolatile", "(Ljava/lang/Object;J)Z", getBooleanVolatile);
    register_native_method(C"getByteVolatile", "(Ljava/lang/Object;J)B", getByteVolatile);
    register_native_method(C"getShortVolatile", "(Ljava/lang/Object;J)S", getShortVolatile);
    register_native_method(C"getCharVolatile", "(Ljava/lang/Object;J)C", getCharVolatile);
    register_native_method(C"getLongVolatile", "(Ljava/lang/Object;J)J", getLongVolatile);
    register_native_method(C"getFloatVolatile", "(Ljava/lang/Object;J)F", getFloatVolatile);
    register_native_method(C"getDoubleVolatile", "(Ljava/lang/Object;J)D", getDoubleVolatile);

    // unsafe memory
    register_native_method(C"allocateMemory", "(J)J", allocateMemory);
    register_native_method(C"reallocateMemory", "(JJ)J", reallocateMemory);
    register_native_method(C"setMemory", "(Ljava/lang/Object;JJB)V", setMemory);
    register_native_method(C"copyMemory", "(Ljava/lang/Object;JLjava/lang/Object;JJ)V", copyMemory);
    register_native_method(C"freeMemory", "(J)V", freeMemory);
    register_native_method(C"addressSize", "()I", addressSize);
    register_native_method(C"putAddress", "(JJ)V", putAddress);
    register_native_method(C"getAddress", "(J)J", getAddress);
    register_native_method(C"putByte", "(JB)V", putByte);
    register_native_method(C"getByte", "(J)B", getByte);
    register_native_method(C"putShort", "(JS)V", putShort);
    register_native_method(C"getShort", "(J)S", getShort);
    register_native_method(C"putChar", "(JC)V", putChar);
    register_native_method(C"getChar", "(J)C", getChar);
    register_native_method(C"putInt", "(JI)V", putInt);
    register_native_method(C"getInt", "(J)I", getInt);
    register_native_method(C"putLong", "(JJ)V", putLong);
    register_native_method(C"getLong", "(J)J", getLong);
    register_native_method(C"putFloat", "(JF)V", putFloat);
    register_native_method(C"getFloat", "(J)F", getFloat);
    register_native_method(C"putDouble", "(JD)V", putDouble);
    register_native_method(C"getDouble", "(J)D", getDouble);

    register_native_method(C"shouldBeInitialized", "(Ljava/lang/Class;)Z", shouldBeInitialized);
    register_native_method(C"getLoadAverage", "([DI)I", getLoadAverage);
    register_native_method(C"pageSize", "()I", pageSize);
    register_native_method(C"defineAnonymousClass", "(Ljava/lang/Class;[B[Ljava/lang/Object;)" LCLS, defineAnonymousClass);
    register_native_method(C"monitorEnter", "(Ljava/lang/Object;)V", monitorEnter);
    register_native_method(C"monitorExit", "(Ljava/lang/Object;)V", monitorExit);
    register_native_method(C"tryMonitorEnter", "(Ljava/lang/Object;)Z", tryMonitorEnter);
    register_native_method(C"throwException", "(Ljava/lang/Throwable;)V", throwException);

    register_native_method(C"loadFence", "()V", loadFence);
    register_native_method(C"storeFence", "()V", storeFence);
    register_native_method(C"fullFence", "()V", fullFence);
}
