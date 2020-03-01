/*
 * Author: kayo
 */

#include <iostream>
#include "../../registry.h"
#include "../../../objects/object.h"
#include "../../../util/endianness.h"
#include "../../../runtime/Frame.h"

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

    jint *old;
    if (o->isArrayObject()) {
        old = (jint *)(((Array *) o)->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = o->data + offset;
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    frame->pushi(b ? 1 : 0);
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(Frame *frame)
{
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2); // long 占两个Slot
    jlong expected = frame->getLocalAsLong(4);
    jlong x = frame->getLocalAsLong(6);

    jlong *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jlong *)(ao->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = (jlong *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);  // todo
    frame->pushi(b ? 1 : 0);
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(Frame *frame)
{
    jref o = frame->getLocalAsRef(1); // first argument
    jlong offset = frame->getLocalAsLong(2); // long 占两个Slot
    jref expected = frame->getLocalAsRef(4);
    jref x = frame->getLocalAsRef(5);

    jref *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jref *)(ao->index(offset));
//        old = arrobj_get(jref, o, offset);
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        old = (jref *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    frame->pushi(b ? 1 : 0);
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
    auto c = frame->getLocalAsRef<Class>(1);
    initClass(c);
//    c->clinit(); // todo 是不是这样搞？
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
    frame->pushi(0);//frame->operandStack.push((jint)0);  // todo
}

// public native int arrayIndexScale(Class<?> type)
static void arrayIndexScale(Frame *frame)
{
    frame->pushi(1); // todo
}

// public native long objectFieldOffset(Field field)
static void objectFieldOffset(Frame *frame)
{
    jref field = frame->getLocalAsRef(1);
    auto offset = field->getInstFieldValue<jint>(S(slot), S(I));
    frame->pushl(offset);
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
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        value = ao->get<jint>(offset);
//        value = arrobj_get(jint, o, offset);  // todo
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        value = o->data[offset];//o->getInstFieldValue<jint>(offset);  // todo
    }
    frame->pushi(value);
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
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        value = ao->get<jref>(offset);
    } else {
        assert(0 <= offset && offset < o->clazz->instFieldsCount);
        value = *(jref *)(o->data + offset);//o->getInstFieldValue<jref>(offset);  // todo
    }
    frame->pushr(value);
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(Frame *frame)
{
    // todo Volatile
    jref o = frame->getLocalAsRef(1);
    jlong offset = frame->getLocalAsLong(2);
    jref x = frame->getLocalAsRef(4);

    if (o->isArrayObject()) {
        auto arr = (Array *) o;
        arr->set(offset, x);
    } else {
        o->setFieldValue(offset, x);
    }
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
    auto p = (u1 *) malloc(sizeof(char)*bytes);
    if (p == nullptr) {
        // todo error
    }
    frame->pushl((jlong) (intptr_t) p);
}

// public native long reallocateMemory(long address, long bytes);
static void reallocateMemory(Frame *frame)
{
    jlong address = frame->getLocalAsLong(1);
    jlong bytes = frame->getLocalAsLong(3);
    frame->pushl((jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes)); // 有内存泄漏
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
    frame->pushi(sizeof(jlong)); // todo
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
    frame->pushi(*(jbyte *) (intptr_t) address);
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
    frame->pushi(*(jchar *) (intptr_t) address);
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
    frame->pushi(*(jshort *) (intptr_t) address);
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
    frame->pushi(*(jint *) (intptr_t) address);
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
    frame->pushl(*(jlong *) (intptr_t) address);
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
    frame->pushf(*(jfloat *) (intptr_t) address);
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
    frame->pushd(*(jdouble *) (intptr_t) address);
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

// (Ljava/lang/Class;)Z
static void shouldBeInitialized(Frame *frame)
{
    // todo
    auto c = frame->getLocalAsRef<Class>(1);
    frame->pushi(c->state >= Class::INITED ? 1 : 0);
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
#undef C
#define C "sun/misc/Unsafe",
#define LCLD "Ljava/lang/ClassLoader;"
    registerNative(C "park", "(ZJ)V", park);
    registerNative(C "unpark", "(Ljava/lang/Object;)V", unpark);

    // compare and swap
    registerNative(C "compareAndSwapInt", "(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    registerNative(C "compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    registerNative(C "compareAndSwapObject", "(" LOBJ "J" LOBJ LOBJ")Z", compareAndSwapObject);

    // class
    registerNative(C "allocateInstance", "(Ljava/lang/Class;)Ljava/lang/Object;", allocateInstance);
    registerNative(C "defineClass", "(" LSTR "[BII" LCLD "Ljava/security/ProtectionDomain;)" LCLS, defineClass);
    registerNative(C "ensureClassInitialized", "(Ljava/lang/Class;)V", ensureClassInitialized);
    registerNative(C "staticFieldOffset", "(Ljava/lang/reflect/Field;)J", staticFieldOffset);
    registerNative(C "staticFieldBase", "(Ljava/lang/reflect/Field;)" LOBJ, staticFieldBase);

    // Object
    registerNative(C "arrayBaseOffset", "(Ljava/lang/Class;)I", arrayBaseOffset);
    registerNative(C "arrayIndexScale", "(Ljava/lang/Class;)I", arrayIndexScale);
    registerNative(C "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", objectFieldOffset);

    registerNative(C "getBoolean", "(Ljava/lang/Object;J)Z", getBoolean);
    registerNative(C "putBoolean", "(Ljava/lang/Object;JZ)V", putBoolean);
    registerNative(C "getByte", "(Ljava/lang/Object;J)B", obj_getByte);
    registerNative(C "putByte", "(Ljava/lang/Object;JB)V", obj_putByte);
    registerNative(C "getChar", "(Ljava/lang/Object;J)C", obj_getChar);
    registerNative(C "putChar", "(Ljava/lang/Object;JC)V", obj_putChar);
    registerNative(C "getShort", "(Ljava/lang/Object;J)S", obj_getShort);
    registerNative(C "putShort", "(Ljava/lang/Object;JS)V", obj_putShort);
    registerNative(C "getInt", "(Ljava/lang/Object;J)I", obj_getInt);
    registerNative(C "putInt", "(Ljava/lang/Object;JI)V", obj_putInt);
    registerNative(C "getLong", "(Ljava/lang/Object;J)J", obj_getLong);
    registerNative(C "putLong", "(Ljava/lang/Object;JJ)V", obj_putLong);
    registerNative(C "getFloat", "(Ljava/lang/Object;J)F", obj_getFloat);
    registerNative(C "putFloat", "(Ljava/lang/Object;JF)V", obj_putFloat);
    registerNative(C "getDouble", "(Ljava/lang/Object;J)D", obj_getDouble);
    registerNative(C "putDouble", "(Ljava/lang/Object;JD)V", obj_putDouble);
    registerNative(C "getObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObject);
    registerNative(C "putObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObject);
    registerNative(C "getObjectVolatile", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObjectVolatile);
    registerNative(C "putObjectVolatile", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObjectVolatile);
    registerNative(C "getOrderedObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getOrderedObject);
    registerNative(C "putOrderedObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putOrderedObject);
    registerNative(C "putOrderedInt", "(Ljava/lang/Object;JI)V", putOrderedInt);
    registerNative(C "putOrderedLong", "(Ljava/lang/Object;JJ)V", putOrderedLong);

    registerNative(C "putIntVolatile", "(Ljava/lang/Object;JI)V", putIntVolatile);
    registerNative(C "putBooleanVolatile", "(Ljava/lang/Object;JZ)V", putBooleanVolatile);
    registerNative(C "putByteVolatile", "(Ljava/lang/Object;JB)V", putByteVolatile);
    registerNative(C "putShortVolatile", "(Ljava/lang/Object;JS)V", putShortVolatile);
    registerNative(C "putCharVolatile", "(Ljava/lang/Object;JC)V", putCharVolatile);
    registerNative(C "putLongVolatile", "(Ljava/lang/Object;JJ)V", putLongVolatile);
    registerNative(C "putFloatVolatile", "(Ljava/lang/Object;JF)V", putFloatVolatile);
    registerNative(C "putDoubleVolatile", "(Ljava/lang/Object;JD)V", putDoubleVolatile);

    registerNative(C "getIntVolatile", "(Ljava/lang/Object;J)I", getIntVolatile);
    registerNative(C "getBooleanVolatile", "(Ljava/lang/Object;J)Z", getBooleanVolatile);
    registerNative(C "getByteVolatile", "(Ljava/lang/Object;J)B", getByteVolatile);
    registerNative(C "getShortVolatile", "(Ljava/lang/Object;J)S", getShortVolatile);
    registerNative(C "getCharVolatile", "(Ljava/lang/Object;J)C", getCharVolatile);
    registerNative(C "getLongVolatile", "(Ljava/lang/Object;J)J", getLongVolatile);
    registerNative(C "getFloatVolatile", "(Ljava/lang/Object;J)F", getFloatVolatile);
    registerNative(C "getDoubleVolatile", "(Ljava/lang/Object;J)D", getDoubleVolatile);

    // unsafe memory
    registerNative(C "allocateMemory", "(J)J", allocateMemory);
    registerNative(C "reallocateMemory", "(JJ)J", reallocateMemory);
    registerNative(C "setMemory", "(Ljava/lang/Object;JJB)V", setMemory);
    registerNative(C "copyMemory", "(Ljava/lang/Object;JLjava/lang/Object;JJ)V", copyMemory);
    registerNative(C "freeMemory", "(J)V", freeMemory);
    registerNative(C "addressSize", "()I", addressSize);
    registerNative(C "putAddress", "(JJ)V", putAddress);
    registerNative(C "getAddress", "(J)J", getAddress);
    registerNative(C "putByte", "(JB)V", putByte);
    registerNative(C "getByte", "(J)B", getByte);
    registerNative(C "putShort", "(JS)V", putShort);
    registerNative(C "getShort", "(J)S", getShort);
    registerNative(C "putChar", "(JC)V", putChar);
    registerNative(C "getChar", "(J)C", getChar);
    registerNative(C "putInt", "(JI)V", putInt);
    registerNative(C "getInt", "(J)I", getInt);
    registerNative(C "putLong", "(JJ)V", putLong);
    registerNative(C "getLong", "(J)J", getLong);
    registerNative(C "putFloat", "(JF)V", putFloat);
    registerNative(C "getFloat", "(J)F", getFloat);
    registerNative(C "putDouble", "(JD)V", putDouble);
    registerNative(C "getDouble", "(J)D", getDouble);

    registerNative(C "shouldBeInitialized", "(Ljava/lang/Class;)Z", shouldBeInitialized);
    registerNative(C "getLoadAverage", "([DI)I", getLoadAverage);
    registerNative(C "pageSize", "()I", pageSize);
    registerNative(C "defineAnonymousClass", "(Ljava/lang/Class;[B[Ljava/lang/Object;)" LCLS, defineAnonymousClass);
    registerNative(C "monitorEnter", "(Ljava/lang/Object;)V", monitorEnter);
    registerNative(C "monitorExit", "(Ljava/lang/Object;)V", monitorExit);
    registerNative(C "tryMonitorEnter", "(Ljava/lang/Object;)Z", tryMonitorEnter);
    registerNative(C "throwException", "(Ljava/lang/Throwable;)V", throwException);

    registerNative(C "loadFence", "()V", loadFence);
    registerNative(C "storeFence", "()V", storeFence);
    registerNative(C "fullFence", "()V", fullFence);
}
