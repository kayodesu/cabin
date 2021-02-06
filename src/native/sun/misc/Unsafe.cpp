#include <cstring>
#include <iostream>
#include "../../../slot.h"
#include "../../../platform/sysinfo.h"
#include "../../../objects/object.h"
#include "../../../objects/array.h"
#include "../../../util/endianness.h"
#include "../../../runtime/frame.h"
#include "../../jni_internal.h"

using namespace std;
using namespace utf8;

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
static void park(jobject _this, jboolean isAbsolute, jlong time)
{
    JVM_PANIC("park");
}

// public native void unpark(Object thread);
static void unpark(jobject _this, jobject thread)
{
    JVM_PANIC("unpark");
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
static jboolean compareAndSwapInt(jobject _this, jobject o, jlong offset, jint expected, jint x)
{
    jint *old;
    if (o->isArrayObject()) {
        old = (jint *)(((Array *) o)->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jint *) (o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? jtrue : jfalse;
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static jboolean compareAndSwapLong(jobject _this, jobject o, jlong offset, jlong expected, jlong x)
{
    jlong *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jlong *)(ao->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jlong *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);  // todo
    return b ? jtrue : jfalse;
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static jboolean compareAndSwapObject(jobject _this, jobject o, jlong offset, jobject expected, jobject x)
{
    jobject *old;
    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  // todo
        old = (jobject *)(ao->index(offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jobject *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? jtrue : jfalse;
}

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static jobject allocateInstance(jobject _this, jclass type)
{
    JVM_PANIC("allocateInstance");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static jclass __defineClass(jobject _this, jstring name, 
                jbyteArray b, jint off, jint len, jobject loader, jobject protectionDomain)
{
    JVM_PANIC("defineClass");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(jobject _this, jclass c)
{
    initClass(c->jvm_mirror);
//    c->clinit(); // todo 是不是这样搞？
}

// public native long staticFieldOffset(Field f);
static jlong staticFieldOffset(jobject _this, jobject f)
{
    // private String name;
    auto name = f->getRefField<ClsObj>("name", "Ljava/lang/String;")->toUtf8();

    // private Class<?> clazz;
    Class *c = f->getRefField<ClsObj>("clazz", "Ljava/lang/Class;")->jvm_mirror;
    for (int i = 0; i < c->fields.size(); i++) {
        Field *field = c->fields[i];
        if (equals(field->name, name))
            return i;
    }

    JVM_PANIC("never go here"); // todo
    return -1; 
}

// public native Object staticFieldBase(Field f);
static jobject staticFieldBase(jobject _this, jobject f)
{
    // private Class<?> clazz;
    auto co = f->getRefField<ClsObj>("clazz", "Ljava/lang/Class;");
    return co;
}

/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static jint arrayBaseOffset(jobject _this, jclass type)
{
    return 0; // todo
}

// public native int arrayIndexScale(Class<?> type)
static jint arrayIndexScale(jobject _this, jclass type)
{
    return 1; // todo
}

// public native long objectFieldOffset(Field field)
static jlong objectFieldOffset(jobject _this, jobject field)
{
    auto offset = field->getIntField(S(slot), S(I));
    return offset;
}

// private native long objectFieldOffset1(Class<?> c, String name);
static jlong objectFieldOffset1(jobject _this, jclass c, jstring name)
{
    JVM_PANIC("objectFieldOffset1");
}

#define OBJECT_PUT(o, offset, x, arrSetFunc, t, slotSetFunc)            \
    do {                                                                \
        if (o->isArrayObject()) { /* set value to array */                   \
            Array *ao = dynamic_cast<Array *>(o);                           \
            assert(0 <= offset && offset < ao->arr_len);                \
            ao->arrSetFunc(offset, x);                                  \
        } else if (o->isClassObject()) { /* set static filed value */        \
            Class *c = o->jvm_mirror;                                  \
            initClass(c);                                               \
            assert(0 <= offset && offset < c->fields.size());           \
            Field *f = c->fields[offset];                              \
            f->static_value.t = x;                                      \
        } else {                                                        \
            assert(0 <= offset && offset < o->clazz->inst_fields_count); \
            slotSetFunc(o->data + offset, x);                           \
        }                                                               \
    } while (false)

#define OBJECT_GET(o, offset, jtype, t, slotGetFunc)                    \
    do {                                                                \
        if (o->isArrayObject()) { /* get value from array */                 \
            Array *ao = dynamic_cast<Array *>(o);                           \
            assert(0 <= offset && offset < ao->arr_len);                \
            return ao->get<jtype>(offset);                              \
        } else if (o->isClassObject()) { /* get static filed value */         \
            Class *c = o->jvm_mirror;                                  \
            initClass(c);                                               \
            assert(0 <= offset && offset < c->fields.size());           \
            Field *f = c->fields[offset];                              \
            return f->static_value.t;                                   \
        } else {                                                        \
            assert(0 <= offset && offset < o->clazz->inst_fields_count); \
            return slotGetFunc(o->data + offset);                       \
        }                                                               \
    } while (false)

// public native boolean getBoolean(Object o, long offset);
static jboolean obj_getBoolean(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jbool, z, slot::getBool);
}

// public native void putBoolean(Object o, long offset, boolean x);
static void obj_putBoolean(jobject _this, jobject o, jlong offset, jboolean x)
{
    OBJECT_PUT(o, offset, x, setBoolean, z, slot::setBool);
}

// public native byte getByte(Object o, long offset);
static jbyte obj_getByte(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jbyte, b, slot::getByte);
}

// public native void putByte(Object o, long offset, byte x);
static void obj_putByte(jobject _this, jobject o, jlong offset, jbyte x)
{
    OBJECT_PUT(o, offset, x, setByte, b, slot::setByte);
}

// public native char getChar(Object o, long offset);
static jchar obj_getChar(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jchar, c, slot::getChar);
}

// public native void putChar(Object o, long offset, char x);
static void obj_putChar(jobject _this, jobject o, jlong offset, jchar x)
{
    OBJECT_PUT(o, offset, x, setChar, c, slot::setChar);
}

// public native short getShort(Object o, long offset);
static jshort obj_getShort(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jshort, s, slot::getShort);
}

// public native void putShort(Object o, long offset, short x);
static void obj_putShort(jobject _this, jobject o, jlong offset, jshort x)
{
    OBJECT_PUT(o, offset, x, setShort, s, slot::setShort);
}

// public native int getInt(Object o, long offset);
static jint obj_getInt(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jint, i, slot::getInt);
}

// public native void putInt(Object o, long offset, int x);
static void obj_putInt(jobject _this, jobject o, jlong offset, jint x)
{
    OBJECT_PUT(o, offset, x, setInt, i, slot::setInt);
}

// public native long getLong(Object o, long offset);
static jlong obj_getLong(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jlong, j, slot::getLong);
}

// public native void putLong(Object o, long offset, long x);
static void obj_putLong(jobject _this, jobject o, jlong offset, jlong x)
{
    OBJECT_PUT(o, offset, x, setLong, j, slot::setLong);
}

// public native float getFloat(Object o, long offset);
static jfloat obj_getFloat(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jfloat, f, slot::getFloat);
}

// public native void putFloat(Object o, long offset, float x);
static void obj_putFloat(jobject _this, jobject o, jlong offset, jfloat x)
{
    OBJECT_PUT(o, offset, x, setFloat, f, slot::setFloat);
}

// public native double getDouble(Object o, long offset);
static jdouble obj_getDouble(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jdouble, d, slot::getDouble);
}

// public native void putDouble(Object o, long offset, double x);
static void obj_putDouble(jobject _this, jobject o, jlong offset, jdouble x)
{
    OBJECT_PUT(o, offset, x, setDouble, d, slot::setDouble);
}

// public native Object getObject(Object o, long offset);
static jobject obj_getObject(jobject _this, jobject o, jlong offset)
{
    OBJECT_GET(o, offset, jref, r, slot::getRef);
}

// public native void putObject(Object o, long offset, Object x);
static void obj_putObject(jobject _this, jobject o, jlong offset, jobject x)
{
    OBJECT_PUT(o, offset, x, setRef, r, slot::setRef);
}

#undef OBJECT_GET
#undef OBJECT_PUT

// public native boolean getBooleanVolatile(Object o, long offset);
static jboolean getBooleanVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getBoolean(_this, o, offset);
    // JVM_PANIC("getBooleanVolatile");
}

// public native byte getByteVolatile(Object o, long offset);
static jbyte getByteVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getByte(_this, o, offset);
    // JVM_PANIC("getByteVolatile");
}

// public native char getCharVolatile(Object o, long offset);
static jchar getCharVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getChar(_this, o, offset);
    // JVM_PANIC("getCharVolatile");
}

// public native short getShortVolatile(Object o, long offset);
static jshort getShortVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getShort(_this, o, offset);
    // JVM_PANIC("getShortVolatile");
}

// public native int getIntVolatile(Object o, long offset);
static jint getIntVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getInt(_this, o, offset);

    // jint value;
    // if (o->isArrayObject()) {
    //     Array *ao = dynamic_cast<Array *>(o);  // todo
    //     value = ao->get<jint>(offset);
    // } else {
    //     assert(0 <= offset && offset < o->clazz->inst_fields_count);
    //     value = o->data[offset];   // todo
    // }
    // return value;
}

// public native long getLongVolatile(Object o, long offset);
static jlong getLongVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getLong(_this, o, offset);
    // JVM_PANIC("getLongVolatile");
}

// public native float getFloatVolatile(Object o, long offset);
static jfloat getFloatVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getFloat(_this, o, offset);
    // JVM_PANIC("getFloatVolatile");
}

// public native double getDoubleVolatile(Object o, long offset);
static jdouble getDoubleVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile
    return obj_getDouble(_this, o, offset);
    // JVM_PANIC("getDoubleVolatile");
}

// public native void putIntVolatile(Object o, long offset, int x);
static void putIntVolatile(jobject _this, jobject o, jlong offset, jint x)
{
    JVM_PANIC("putIntVolatile");
}

// public native void putBooleanVolatile(Object o, long offset, boolean x);
static void putBooleanVolatile(jobject _this, jobject o, jlong offset, jboolean x)
{
    JVM_PANIC("putBooleanVolatile");
}

// public native void putByteVolatile(Object o, long offset, byte x);
static void putByteVolatile(jobject _this, jobject o, jlong offset, jbyte x)
{
    JVM_PANIC("putByteVolatile");
}

// public native void putShortVolatile(Object o, long offset, short x);
static void putShortVolatile(jobject _this, jobject o, jlong offset, jshort x)
{
    JVM_PANIC("putShortVolatile");
}

// public native void putCharVolatile(Object o, long offset, char x);
static void putCharVolatile(jobject _this, jobject o, jlong offset, jchar x)
{
    JVM_PANIC("putCharVolatile");
}

// public native void putLongVolatile(Object o, long offset, long x);
static void putLongVolatile(jobject _this, jobject o, jlong offset, jlong x)
{
    JVM_PANIC("putLongVolatile");
}

// public native void putFloatVolatile(Object o, long offset, float x);
static void putFloatVolatile(jobject _this, jobject o, jlong offset, jfloat x)
{
    JVM_PANIC("putFloatVolatile");
}

// public native void putDoubleVolatile(Object o, long offset, double x);
static void putDoubleVolatile(jobject _this, jobject o, jlong offset, jdouble x)
{
    JVM_PANIC("putDoubleVolatile");
}
// -------------------------
// public native Object getObjectVolatile(Object o, long offset);
static jobject getObjectVolatile(jobject _this, jobject o, jlong offset)
{
    // todo Volatile

    if (o->isArrayObject()) {
        Array *ao = dynamic_cast<Array *>(o);  
        return ao->get<jobject>(offset);
    } else if (o->isClassObject()) {
        Class *c = o->jvm_mirror;
        assert(0 <= offset && offset < c->fields.size());
        Field *f = c->fields[offset];
        return f->static_value.r;
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        return *(jobject *)(o->data + offset);//o->getInstFieldValue<jobject>(offset);  // todo
    }
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(jobject _this, jobject o, jlong offset, jobject x)
{
    // todo Volatile
    if (o->isArrayObject()) {
        auto arr = (Array *) o;
        arr->setRef(offset, x);
    } else {
        o->setFieldValue(offset, x);
    }
}

// public native Object getOrderedObject(Object o, long offset);
static void getOrderedObject(jobject _this, jobject o, jlong offset)
{
    JVM_PANIC("getOrderedObject");
}

// public native void putOrderedObject(Object o, long offset, Object x);
static void putOrderedObject(jobject _this, jobject o, jlong offset, jobject x)
{
    JVM_PANIC("putOrderedObject");
}

/** Ordered/Lazy version of {@link #putIntVolatile(Object, long, int)}  */
// public native void putOrderedInt(Object o, long offset, int x);
static void putOrderedInt(jobject _this, jobject o, jlong offset, jint x)
{
    JVM_PANIC("putOrderedInt");
}

/** Ordered/Lazy version of {@link #putLongVolatile(Object, long, long)} */
// public native void putOrderedLong(Object o, long offset, long x);
static void putOrderedLong(jobject _this, jobject o, jlong offset, jlong x)
{
    JVM_PANIC("putOrderedLong");
}

/*************************************    unsafe memory    ************************************/
// todo 说明 unsafe memory

/*
 * todo
 * 分配内存方法还有重分配内存方法都是分配的堆外内存，
 * 返回的是一个long类型的地址偏移量。这个偏移量在你的Java程序中每块内存都是唯一的。
 */
// public native long allocateMemory(long bytes);
static jlong allocateMemory(jobject _this, jlong bytes)
{
    auto p = (u1 *) malloc(sizeof(char)*bytes);
    if (p == nullptr) {
        // todo error
    }
    return (jlong) (intptr_t) p;
}

// public native long reallocateMemory(long address, long bytes);
static jlong reallocateMemory(jobject _this, jlong address, jlong bytes)
{
    return (jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes); // 有内存泄漏  todo
}

// public native void freeMemory(long address);
static void freeMemory(jobject _this, jlong address)
{
    free((void *) (intptr_t) address);
}

// public native int addressSize();
static jint addressSize(jobject _this)
{
    return sizeof(jlong); // todo
}

// public native void putByte(long address, byte x);
static void putByte(jobject _this, jlong address, jbyte x)
{
    *(jbyte *) (intptr_t) address = x; 
}

// public native byte getByte(long address);
static jbyte getByte(jobject _this, jlong address)
{
    return *(jbyte *) (intptr_t) address;
}

// public native void putChar(long address, char x);
static void putChar(jobject _this, jlong address, jchar x)
{
    *(jchar *) (intptr_t) address = x;
}

// public native char getChar(long address);
static jchar getChar(jobject _this, jlong address)
{
    return *(jchar *) (intptr_t) address;
}

// public native void putShort(long address, short x);
static void putShort(jobject _this, jlong address, jshort x)
{
    *(jshort *) (intptr_t) address = x;
}

// public native short getShort(long address);
static jshort getShort(jobject _this, jlong address)
{
    return *(jshort *) (intptr_t) address;
}

// public native void putInt(long address, int x);
static void putInt(jobject _this, jlong address, jint x)
{
    *(jint *) (intptr_t) address = x;
}

// public native int getInt(long address);
static jint getInt(jobject _this, jlong address)
{
    return *(jint *) (intptr_t) address;
}

// public native void putLong(long address, long x);
static void putLong(jobject _this, jlong address, jlong x)
{
    *(jlong *) (intptr_t) address = x;
}

// public native long getLong(long address);
static jlong getLong(jobject _this, jlong address)
{
    return *(jlong *) (intptr_t) address;
}

// public native void putFloat(long address, float x);
static void putFloat(jobject _this, jlong address, jfloat x)
{
    *(jfloat *) (intptr_t) address = x;
}

// public native float getFloat(long address);
static jfloat getFloat(jobject _this, jlong address)
{
    return *(jfloat *) (intptr_t) address;
}

// public native void putDouble(long address, double x);
static void putDouble(jobject _this, jlong address, jdouble x)
{
    *(jdouble *) (intptr_t) address = x;
}

// public native double getDouble(long address);
static jdouble getDouble(jobject _this, jlong address)
{
    return *(jdouble *) (intptr_t) address;
}

// public native void putAddress(long address, long x);
static void putAddress(jobject _this, jlong address, jlong x)
{
    putLong(_this, address, x);
}

// public native long getAddress(long address);
static jlong getAddress(jobject _this, jlong address)
{
    return getLong(_this, address);
}

static void *_getPoint(jobject o, jlong offset)
{
    void *p = nullptr;

    if (o == nullptr) {
        p = (void *) (intptr_t) offset;
    } else {
        Array *ao = dynamic_cast<Array *>(o);
        if (ao != nullptr) {
            // offset 在这里表示数组下标(index)
            p = ao->index(offset);
        } else {
            // offset 在这里表示 slot id.
            p = o->data;
        }
    }

    return p;
}

/**
 * Sets all bytes in a given block of memory to a fixed value (usually zero).
 * 
 * This method determines a block's base address by means of two parameters,
 * and so it provides (in effect) a <em>double-register</em> addressing mode,
 * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
 * the offset supplies an absolute base address.
 *
 * The stores are in coherent (atomic) units of a size determined
 * by the address and length parameters.  If the effective address and
 * length are all even modulo 8, the stores take place in 'long' units.
 * If the effective address and length are (resp.) even modulo 4 or 2,
 * the stores take place in units of 'int' or 'short'.
 */
// public native void setMemory(Object o, long offset, long bytes, byte value);
static void setMemory(jobject _this, jobject o, jlong offset, jlong bytes, jbyte value)
{
    void *p = _getPoint(o, offset);
    assert(p != nullptr);
    // さすが Unsafe class. This is too unsafe.
    memset(p, value, bytes);
}

/**
 * Sets all bytes in a given block of memory to a copy of another block.
 *
 * This method determines each block's base address by means of two parameters,
 * and so it provides (in effect) a <em>double-register</em> addressing mode,
 * as discussed in {@link #getInt(Object,long)}.  When the object reference is null,
 * the offset supplies an absolute base address.
 *
 * The transfers are in coherent (atomic) units of a size determined
 * by the address and length parameters.  If the effective addresses and
 * length are all even modulo 8, the transfer takes place in 'long' units.
 * If the effective addresses and length are (resp.) even modulo 4 or 2,
 * the transfer takes place in units of 'int' or 'short'.
 */
// public native void copyMemory(Object srcBase, long srcOffset, Object destBase, long destOffset, long bytes);
static void copyMemory(jobject _this, 
                    jobject src_base, jlong src_offset, 
                    jobject dest_base, jlong dest_offset, jlong bytes)
{
    void *src_p = _getPoint(src_base, src_offset);
    void *dest_p = _getPoint(dest_base, dest_offset);

    assert(src_p != nullptr);
    assert(dest_p != nullptr);
    memcpy(dest_p, src_p, bytes);
}

/**
* Gets the load average in the system run queue assigned
* to the available processors averaged over various periods of time.
* This method retrieves the given <tt>nelem</tt> samples and
* assigns to the elements of the given <tt>loadavg</tt> array.
* The system imposes a maximum of 3 samples, representing
* averages over the last 1,  5,  and  15 minutes, respectively.
*
* @params loadavg an array of double of size nelems
* @params nelems the number of samples to be retrieved and
*         must be 1 to 3.
*
* @return the number of samples actually retrieved; or -1
*         if the load average is unobtainable.
*/
// public native int getLoadAverage(double[] loadavg, int nelems);
static jint getLoadAverage(jobject _this, jdoubleArray loadavg, jint nelems)
{
    JVM_PANIC("getLoadAverage"); // todo
}

// (Ljava/lang/Class;)Z
static jboolean shouldBeInitialized(jobject _this, jclass c)
{
    // todo
    return c->jvm_mirror->state >= Class::INITED ? jtrue : jfalse;
}

/**
 * Report the size in bytes of a native memory page (whatever that is).
 * This value will always be a power of two.
 * 
 * public native int pageSize();
*/
static jint _pageSize(jobject _this)
{
    return pageSize();
}

/**
  * Define a class but do not make it known to the class loader or system dictionary.
  * 
  * For each CP entry, the corresponding CP patch must either be null or have
  * the a format that matches its tag:
  * 1. Integer, Long, Float, Double: the corresponding wrapper object type from java.lang
  * 2. Utf8: a string (must have suitable syntax if used as signature or name)
  * 3. Class: any java.lang.Class object
  * 4. String: any object (not just a java.lang.String)
  * 5. InterfaceMethodRef: (NYI) a method handle to invoke on that call site's arguments
  * 
  * @hostClass context for linkage, access control, protection domain, and class loader
  * @data      bytes of a class file
  * @cpPatches where non-null entries exist, they replace corresponding CP entries in data
  */
// public native Class defineAnonymousClass(Class hostClass, byte[] data, Object[] cpPatches);
static jclass defineAnonymousClass(jobject _this, 
                    jclass host_class, jbyteArray data, jobjectArray cp_patches)
{
    assert(host_class != nullptr && data != nullptr);
    Class *c = defineClass(host_class->jvm_mirror->loader, (u1 *) data->data, data->arr_len);
    if (c == nullptr)
        return nullptr; // todo

    int cp_patches_len = cp_patches == nullptr ? 0 : cp_patches->arr_len;
    for (int i = 0; i < cp_patches_len; i++) {
        jobject o = cp_patches->get<jobject>(i);
        if (o != nullptr) {
            u1 type = c->cp.type(i);
            if (type == JVM_CONSTANT_String) {
                c->cp.info(i, (slot_t) o);
                c->cp.type(i, JVM_CONSTANT_ResolvedString);
            } else {
                JVM_PANIC("defineAnonymousClass: unimplemented patch type"); // todo
            }
        }
    }

    c->nest_host = host_class->jvm_mirror;
    linkClass(c);

    return c->java_mirror;
}

/** Lock the object.  It must get unlocked via {@link #monitorExit}. */
//  public native void monitorEnter(Object o);
static void monitorEnter(jobject _this, jobject o)
{
    JVM_PANIC("monitorEnter"); // todo
}

/**
 * Unlock the object.  It must have been locked via monitorEnter.
 * 
 * public native void monitorExit(Object o);
 */
static void monitorExit(jobject _this, jobject o)
{
    JVM_PANIC("monitorExit"); // todo
}

/**
  848        * Tries to lock the object.  Returns true or false to indicate
  849        * whether the lock succeeded.  If it did, the object must be
  850        * unlocked via {@link #monitorExit}.
  851        */
// public native boolean tryMonitorEnter(Object o);
static jboolean tryMonitorEnter(jobject _this, jobject o)
{
    JVM_PANIC("tryMonitorEnter"); // todo
}

/** Throw the exception without telling the verifier. */
// public native void throwException(Throwable ee);
static void throwException(jobject _this, jobject ee)
{
    JVM_PANIC("throwException"); // todo
}

// "()V"
static void loadFence(jobject _this)
{
    JVM_PANIC("loadFence"); // todo
}

// "()V"
static void storeFence(jobject _this)
{
    JVM_PANIC("storeFence"); // todo
}

// "()V"
static void fullFence(jobject _this)
{
    JVM_PANIC("fullFence"); // todo
}

#undef CLD
#define CLD "Ljava/lang/ClassLoader;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "park", "(ZJ)V", (void *) park },
        { "unpark", _OBJ_ "V", (void *) unpark },

        // compare and swap
        { "compareAndSwapInt", _OBJ "JII)Z", (void *) compareAndSwapInt },
        { "compareAndSwapLong", _OBJ "JJJ)Z", (void *) compareAndSwapLong },
        { "compareAndSwapObject", _OBJ "J" OBJ OBJ_ "Z", (void *) compareAndSwapObject },

        // class
        { "allocateInstance", _CLS_ OBJ, (void *) allocateInstance },
        { "defineClass", _STR "[BII" CLD "Ljava/security/ProtectionDomain;)" CLS, (void *) __defineClass },
        { "ensureClassInitialized", _CLS_ "V", (void *) ensureClassInitialized },
        { "staticFieldOffset", "(Ljava/lang/reflect/Field;)J", (void *) staticFieldOffset },
        { "staticFieldBase", "(Ljava/lang/reflect/Field;)" OBJ, (void *) staticFieldBase },

        // Object
        { "arrayBaseOffset", _CLS_"I", (void *) arrayBaseOffset },
        { "arrayIndexScale", _CLS_"I", (void *) arrayIndexScale },
        { "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", (void *) objectFieldOffset },
        { "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", (void *) objectFieldOffset },
        { "objectFieldOffset1", _CLS STR_ "J", (void *) objectFieldOffset1 },

        { "getBoolean", _OBJ "J)Z", (void *) obj_getBoolean },
        { "putBoolean", _OBJ "JZ)V", (void *) obj_putBoolean },
        { "getByte", _OBJ "J)B", (void *) obj_getByte },
        { "putByte", _OBJ "JB)V", (void *) obj_putByte },
        { "getChar", _OBJ "J)C", (void *) obj_getChar },
        { "putChar", _OBJ "JC)V", (void *) obj_putChar },
        { "getShort", _OBJ "J)S", (void *) obj_getShort },
        { "putShort", _OBJ "JS)V", (void *) obj_putShort },
        { "getInt", _OBJ "J)I", (void *) obj_getInt },
        { "putInt", _OBJ "JI)V", (void *) obj_putInt },
        { "getLong", _OBJ "J)J", (void *) obj_getLong },
        { "putLong", _OBJ "JJ)V", (void *) obj_putLong },
        { "getFloat", _OBJ "J)F", (void *) obj_getFloat },
        { "putFloat", _OBJ "JF)V", (void *) obj_putFloat },
        { "getDouble", _OBJ "J)D", (void *) obj_getDouble },
        { "putDouble", _OBJ "JD)V", (void *) obj_putDouble },
        { "getObject", _OBJ "J)" OBJ, (void *) obj_getObject },
        { "putObject", _OBJ "J" OBJ_ "V", (void *) obj_putObject },
        { "getObjectVolatile", _OBJ "J)" OBJ, (void *) getObjectVolatile },
        { "putObjectVolatile", _OBJ "J" OBJ_ "V", (void *) putObjectVolatile },
        { "getOrderedObject", _OBJ "J)" OBJ, (void *) getOrderedObject },
        { "putOrderedObject", _OBJ "J" OBJ_ "V", (void *) putOrderedObject },
        { "putOrderedInt", _OBJ "JI)V", (void *) putOrderedInt },
        { "putOrderedLong", _OBJ "JJ)V", (void *) putOrderedLong },

        { "putIntVolatile", _OBJ "JI)V", (void *) putIntVolatile },
        { "putBooleanVolatile", _OBJ "JZ)V", (void *) putBooleanVolatile },
        { "putByteVolatile", _OBJ "JB)V", (void *) putByteVolatile },
        { "putShortVolatile", _OBJ "JS)V", (void *) putShortVolatile },
        { "putCharVolatile", _OBJ "JC)V", (void *) putCharVolatile },
        { "putLongVolatile", _OBJ "JJ)V", (void *) putLongVolatile },
        { "putFloatVolatile", _OBJ "JF)V", (void *) putFloatVolatile },
        { "putDoubleVolatile", _OBJ "JD)V", (void *) putDoubleVolatile },

        { "getCharVolatile", _OBJ "J)C", (void *) getCharVolatile },
        { "getIntVolatile", _OBJ "J)I", (void *) getIntVolatile },
        { "getBooleanVolatile", _OBJ "J)Z", (void *) getBooleanVolatile },
        { "getByteVolatile", _OBJ "J)B", (void *) getByteVolatile },
        { "getShortVolatile", _OBJ "J)S", (void *) getShortVolatile },
        { "getLongVolatile", _OBJ "J)J", (void *) getLongVolatile },
        { "getFloatVolatile", _OBJ "J)F", (void *) getFloatVolatile },
        { "getDoubleVolatile", _OBJ "J)D", (void *) getDoubleVolatile },

        // unsafe memory
        { "allocateMemory", "(J)J", (void *) allocateMemory },
        { "reallocateMemory", "(JJ)J", (void *) reallocateMemory },
        { "setMemory", _OBJ "JJB)V", (void *) setMemory },
        { "copyMemory", _OBJ "JLjava/lang/Object;JJ)V", (void *) copyMemory },
        { "freeMemory", "(J)V", (void *) freeMemory },
        { "addressSize", "()I", (void *) addressSize },
        { "putAddress", "(JJ)V", (void *) putAddress },
        { "getAddress", "(J)J", (void *) getAddress },
        { "putByte", "(JB)V", (void *) putByte },
        { "getByte", "(J)B", (void *) getByte },
        { "putShort", "(JS)V", (void *) putShort },
        { "getShort", "(J)S", (void *) getShort },
        { "putChar", "(JC)V", (void *) putChar },
        { "getChar", "(J)C", (void *) getChar },
        { "putInt", "(JI)V", (void *) putInt },
        { "getInt", "(J)I", (void *) getInt },
        { "putLong", "(JJ)V", (void *) putLong },
        { "getLong", "(J)J", (void *) getLong },
        { "putFloat", "(JF)V", (void *) putFloat },
        { "getFloat", "(J)F", (void *) getFloat },
        { "putDouble", "(JD)V", (void *) putDouble },
        { "getDouble", "(J)D", (void *) getDouble },

        { "shouldBeInitialized", _CLS_ "Z", (void *) shouldBeInitialized },
        { "getLoadAverage", "([DI)I", (void *) getLoadAverage },
        { "pageSize", "()I", (void *) _pageSize },
        { "defineAnonymousClass", _CLS "[B[" OBJ_ CLS, (void *) defineAnonymousClass },
        { "monitorEnter", _OBJ_ "V", (void *) monitorEnter },
        { "monitorExit", _OBJ_ "V", (void *) monitorExit },
        { "tryMonitorEnter", _OBJ_ "Z", (void *) tryMonitorEnter },
        { "throwException", "(Ljava/lang/Throwable;)V", (void *) throwException },

        { "loadFence", "()V", (void *) loadFence },
        { "storeFence", "()V", (void *) storeFence },
        { "fullFence", "()V", (void *) fullFence },
};

void sun_misc_Unsafe_registerNatives()
{
    registerNatives("sun/misc/Unsafe", methods, ARRAY_LENGTH(methods));

    registerNatives("jdk/internal/misc/Unsafe", methods, ARRAY_LENGTH(methods));
}
