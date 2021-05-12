#include <string.h>
#include "../../../../slot.h"
#include "../../../../platform/sysinfo.h"
#include "../../../../objects/object.h"
#include "../../../../runtime/frame.h"
#include "../../../jni_internal.h"
#include "../../../../util/endianness.h"


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
static void park(JNIEnv *env, jref _this, jboolean isAbsolute, jlong time)
{
    JVM_PANIC("park");
}

// public native void unpark(Object thread);
static void unpark(JNIEnv *env, jref _this, jref thread)
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
static jboolean compareAndSwapInt(JNIEnv *env, jref _this, jref o, jlong offset, jint expected, jint x)
{
    jint *old;
    if (is_array_object(o)) {
        old = (jint *)(array_index(o, offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jint *) (o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? jtrue : jfalse;
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static jboolean compareAndSwapLong(JNIEnv *env, jref _this, jref o, jlong offset, jlong expected, jlong x)
{
    jlong *old;
    if (is_array_object(o)) {
        old = (jlong *)(array_index(o, offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jlong *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);  // todo
    return b ? jtrue : jfalse;
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static jboolean compareAndSwapObject(JNIEnv *env, jref _this, jref o, jlong offset, jref expected, jref x)
{
    jref *old;
    if (is_array_object(o)) {
        assert(o != NULL);
        old = (jref *)(array_index(o, offset));
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        old = (jref *)(o->data + offset);
    }

    bool b = __sync_bool_compare_and_swap(old, expected, x);
    return b ? jtrue : jfalse;
}

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static jref allocateInstance(JNIEnv *env, jref _this, jclsref type)
{
    JVM_PANIC("allocateInstance");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static jclsref defineClass(JNIEnv *env, jref _this, jstrref name,
                             jref b, jint off, jint len, jref loader, jref protectionDomain)
{
    JVM_PANIC("define_class");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(JNIEnv *env, jref _this, jclsref c)
{
    init_class(c->jvm_mirror);
//    c->clinit(); // todo 是不是这样搞？
}

// public native long staticFieldOffset(Field f);
static jlong staticFieldOffset(JNIEnv *env, jref _this, jref f)
{
    // private String name;
    utf8_t *name = string_to_utf8(get_ref_field(f, "name", "Ljava/lang/String;"));

    // private Class<?> clazz;
    Class *c = get_ref_field(f, "clazz", "Ljava/lang/Class;")->jvm_mirror;
    for (int i = 0; i < c->fields_count; i++) {
        // Field *field = c->fields[i];
        if (utf8_equals(c->fields[i].name, name))
            return i;
    }

    JVM_PANIC("never go here"); // todo
    return -1; 
}

// public native Object staticFieldBase(Field f);
static jref staticFieldBase(JNIEnv *env, jref _this, jref f)
{
    // private Class<?> clazz;
    jclsref co = get_ref_field(f, "clazz", "Ljava/lang/Class;");
    return co;
}

/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static jint arrayBaseOffset(JNIEnv *env, jref _this, jclsref type)
{
    return 0; // todo
}

// public native int arrayIndexScale(Class<?> type)
static jint arrayIndexScale(JNIEnv *env, jref _this, jclsref type)
{
    return 1; // todo
}

// public native long objectFieldOffset(Field field)
static jlong objectFieldOffset(JNIEnv *env, jref _this, jref field)
{
    jint offset = get_int_field(field, S(slot));
    return offset;
}

// private native long objectFieldOffset1(Class<?> c, String name);
static jlong objectFieldOffset1(JNIEnv *env, jref _this, jclsref c, jstrref name)
{
    Field *f = get_declared_field(c->jvm_mirror, string_to_utf8(name));
    return f->id;
}

#define OBJECT_PUT(o, offset, x, arr_set_func, t, slotSetFunc) \
    do { \
        if (is_array_object(o)) { /* set value to array */ \
            assert(0 <= offset && offset < o->arr_len); \
            arr_set_func(o, offset, x); \
        } else if (is_class_object(o)) { /* set static filed value */ \
            Class *c = o->jvm_mirror; \
            init_class(c); \
            assert(0 <= offset && offset < c->fields_count); \
            Field *f = c->fields + offset; \
            f->static_value.t = x; \
        } else { \
            assert(0 <= offset && offset < o->clazz->inst_fields_count); \
            slotSetFunc(o->data + offset, x); \
        } \
    } while (false)

#define OBJECT_GET(o, offset, jtype, t, slotGetFunc) \
    do {  \
        if (is_array_object(o)) { /* get value from array */ \
            assert(0 <= offset && offset < o->arr_len); \
            return array_get(jtype, o, offset); \
        } else if (is_class_object(o)) { /* get static filed value */ \
            Class *c = o->jvm_mirror; \
            init_class(c);  \
            assert(0 <= offset && offset < c->fields_count); \
            Field *f = c->fields + offset; \
            return f->static_value.t; \
        } else { \
            assert(0 <= offset && offset < o->clazz->inst_fields_count); \
            return slotGetFunc(o->data + offset); \
        } \
    } while (false)

// public native boolean getBoolean(Object o, long offset);
static jboolean obj_getBoolean(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jbool, z, slot_get_bool);
}

// public native void putBoolean(Object o, long offset, boolean x);
static void obj_putBoolean(JNIEnv *env, jref _this, jref o, jlong offset, jboolean x)
{
    OBJECT_PUT(o, offset, x, array_set_boolean, z, slot_set_bool);
}

// public native byte getByte(Object o, long offset);
static jbyte obj_getByte(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jbyte, b, slot_get_byte);
}

// public native void putByte(Object o, long offset, byte x);
static void obj_putByte(JNIEnv *env, jref _this, jref o, jlong offset, jbyte x)
{
    OBJECT_PUT(o, offset, x, array_set_byte, b, slot_set_byte);
}

// public native char getChar(Object o, long offset);
static jchar obj_getChar(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jchar, c, slot_get_char);
}

// public native void putChar(Object o, long offset, char x);
static void obj_putChar(JNIEnv *env, jref _this, jref o, jlong offset, jchar x)
{
    OBJECT_PUT(o, offset, x, array_set_char, c, slot_set_char);
}

// public native short getShort(Object o, long offset);
static jshort obj_getShort(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jshort, s, slot_get_short);
}

// public native void putShort(Object o, long offset, short x);
static void obj_putShort(JNIEnv *env, jref _this, jref o, jlong offset, jshort x)
{
    OBJECT_PUT(o, offset, x, array_set_short, s, slot_set_short);
}

// public native int getInt(Object o, long offset);
static jint obj_getInt(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jint, i, slot_get_int);
}

// public native void putInt(Object o, long offset, int x);
static void obj_putInt(JNIEnv *env, jref _this, jref o, jlong offset, jint x)
{
    OBJECT_PUT(o, offset, x, array_set_int, i, slot_set_int);
}

// public native long getLong(Object o, long offset);
static jlong obj_getLong(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jlong, j, slot_get_long);
}

// public native void putLong(Object o, long offset, long x);
static void obj_putLong(JNIEnv *env, jref _this, jref o, jlong offset, jlong x)
{
    OBJECT_PUT(o, offset, x, array_set_long, j, slot_set_long);
}

// public native float getFloat(Object o, long offset);
static jfloat obj_getFloat(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jfloat, f, slot_get_float);
}

// public native void putFloat(Object o, long offset, float x);
static void obj_putFloat(JNIEnv *env, jref _this, jref o, jlong offset, jfloat x)
{
    OBJECT_PUT(o, offset, x, array_set_float, f, slot_set_float);
}

// public native double getDouble(Object o, long offset);
static jdouble obj_getDouble(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jdouble, d, slot_get_double);
}

// public native void putDouble(Object o, long offset, double x);
static void obj_putDouble(JNIEnv *env, jref _this, jref o, jlong offset, jdouble x)
{
    OBJECT_PUT(o, offset, x, array_set_double, d, slot_set_double);
}

// public native Object getObject(Object o, long offset);
static jref obj_getObject(JNIEnv *env, jref _this, jref o, jlong offset)
{
    OBJECT_GET(o, offset, jref, r, slot_get_ref);
}

// public native void putObject(Object o, long offset, Object x);
static void obj_putObject(JNIEnv *env, jref _this, jref o, jlong offset, jref x)
{
    OBJECT_PUT(o, offset, x, array_set_ref, r, slot_set_ref);
}

#undef OBJECT_GET
#undef OBJECT_PUT

// public native boolean getBooleanVolatile(Object o, long offset);
static jboolean getBooleanVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getBoolean(env, _this, o, offset);
    // JVM_PANIC("getBooleanVolatile");
}

// public native byte getByteVolatile(Object o, long offset);
static jbyte getByteVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getByte(env, _this, o, offset);
    // JVM_PANIC("getByteVolatile");
}

// public native char getCharVolatile(Object o, long offset);
static jchar getCharVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getChar(env, _this, o, offset);
    // JVM_PANIC("getCharVolatile");
}

// public native short getShortVolatile(Object o, long offset);
static jshort getShortVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getShort(env, _this, o, offset);
    // JVM_PANIC("getShortVolatile");
}

// public native int getIntVolatile(Object o, long offset);
static jint getIntVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getInt(env, _this, o, offset);

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
static jlong getLongVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getLong(env, _this, o, offset);
    // JVM_PANIC("getLongVolatile");
}

// public native float getFloatVolatile(Object o, long offset);
static jfloat getFloatVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getFloat(env, _this, o, offset);
    // JVM_PANIC("getFloatVolatile");
}

// public native double getDoubleVolatile(Object o, long offset);
static jdouble getDoubleVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile
    return obj_getDouble(env, _this, o, offset);
    // JVM_PANIC("getDoubleVolatile");
}

// public native void putIntVolatile(Object o, long offset, int x);
static void putIntVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jint x)
{
    JVM_PANIC("putIntVolatile");
}

// public native void putBooleanVolatile(Object o, long offset, boolean x);
static void putBooleanVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jboolean x)
{
    JVM_PANIC("putBooleanVolatile");
}

// public native void putByteVolatile(Object o, long offset, byte x);
static void putByteVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jbyte x)
{
    JVM_PANIC("putByteVolatile");
}

// public native void putShortVolatile(Object o, long offset, short x);
static void putShortVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jshort x)
{
    JVM_PANIC("putShortVolatile");
}

// public native void putCharVolatile(Object o, long offset, char x);
static void putCharVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jchar x)
{
    JVM_PANIC("putCharVolatile");
}

// public native void putLongVolatile(Object o, long offset, long x);
static void putLongVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jlong x)
{
    JVM_PANIC("putLongVolatile");
}

// public native void putFloatVolatile(Object o, long offset, float x);
static void putFloatVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jfloat x)
{
    JVM_PANIC("putFloatVolatile");
}

// public native void putDoubleVolatile(Object o, long offset, double x);
static void putDoubleVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jdouble x)
{
    JVM_PANIC("putDoubleVolatile");
}

// -------------------------
// public native Object getObjectVolatile(Object o, long offset);
static jref getObjectVolatile(JNIEnv *env, jref _this, jref o, jlong offset)
{
    // todo Volatile

    if (is_array_object(o)) {
        return array_get(jref, o, offset);
    } else if (is_class_object(o)) {
        Class *c = o->jvm_mirror;
        assert(0 <= offset && offset < c->fields_count);
        // Field *f = c->fields[offset];
        return c->fields[offset].static_value.r;
    } else {
        assert(0 <= offset && offset < o->clazz->inst_fields_count);
        return *(jref *)(o->data + offset);//o->getInstFieldValue<jref>(offset);  // todo
    }
}

// public native void putObjectVolatile(Object o, long offset, Object x);
static void putObjectVolatile(JNIEnv *env, jref _this, jref o, jlong offset, jref x)
{
    // todo Volatile
    if (is_array_object(o)) {
        array_set_ref(o, offset, x);
    } else {
        set_field_value1(o, offset, x);
    }
}

// public native Object getOrderedObject(Object o, long offset);
static void getOrderedObject(JNIEnv *env, jref _this, jref o, jlong offset)
{
    JVM_PANIC("getOrderedObject");
}

// public native void putOrderedObject(Object o, long offset, Object x);
static void putOrderedObject(JNIEnv *env, jref _this, jref o, jlong offset, jref x)
{
    JVM_PANIC("putOrderedObject");
}

/** Ordered/Lazy version of {@link #putIntVolatile(Object, long, int)}  */
// public native void putOrderedInt(Object o, long offset, int x);
static void putOrderedInt(JNIEnv *env, jref _this, jref o, jlong offset, jint x)
{
    JVM_PANIC("putOrderedInt");
}

/** Ordered/Lazy version of {@link #putLongVolatile(Object, long, long)} */
// public native void putOrderedLong(Object o, long offset, long x);
static void putOrderedLong(JNIEnv *env, jref _this, jref o, jlong offset, jlong x)
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
static jlong allocateMemory(JNIEnv *env, jref _this, jlong bytes)
{
    u1 *p = vm_malloc(sizeof(char)*bytes);
    if (p == NULL) {
        // todo error
    }
    return (jlong) (intptr_t) p;
}

// public native long reallocateMemory(long address, long bytes);
static jlong reallocateMemory(JNIEnv *env, jref _this, jlong address, jlong bytes)
{
    return (jlong) (intptr_t) realloc((void *) (intptr_t) address, (size_t) bytes); // 有内存泄漏  todo
}

// public native void freeMemory(long address);
static void freeMemory(JNIEnv *env, jref _this, jlong address)
{
    free((void *) (intptr_t) address);
}

// public native int addressSize();
static jint addressSize(JNIEnv *env, jref _this)
{
    return sizeof(jlong); // todo
}

// public native void putByte(long address, byte x);
static void putByte(JNIEnv *env, jref _this, jlong address, jbyte x)
{
    *(jbyte *) (intptr_t) address = x; 
}

// public native byte getByte(long address);
static jbyte getByte(JNIEnv *env, jref _this, jlong address)
{
    return *(jbyte *) (intptr_t) address;
}

// public native void putChar(long address, char x);
static void putChar(JNIEnv *env, jref _this, jlong address, jchar x)
{
    *(jchar *) (intptr_t) address = x;
}

// public native char getChar(long address);
static jchar getChar(JNIEnv *env, jref _this, jlong address)
{
    return *(jchar *) (intptr_t) address;
}

// public native void putShort(long address, short x);
static void putShort(JNIEnv *env, jref _this, jlong address, jshort x)
{
    *(jshort *) (intptr_t) address = x;
}

// public native short getShort(long address);
static jshort getShort(JNIEnv *env, jref _this, jlong address)
{
    return *(jshort *) (intptr_t) address;
}

// public native void putInt(long address, int x);
static void putInt(JNIEnv *env, jref _this, jlong address, jint x)
{
    *(jint *) (intptr_t) address = x;
}

// public native int getInt(long address);
static jint getInt(JNIEnv *env, jref _this, jlong address)
{
    return *(jint *) (intptr_t) address;
}

// public native void putLong(long address, long x);
static void putLong(JNIEnv *env, jref _this, jlong address, jlong x)
{
    *(jlong *) (intptr_t) address = x;
}

// public native long getLong(long address);
static jlong getLong(JNIEnv *env, jref _this, jlong address)
{
    return *(jlong *) (intptr_t) address;
}

// public native void putFloat(long address, float x);
static void putFloat(JNIEnv *env, jref _this, jlong address, jfloat x)
{
    *(jfloat *) (intptr_t) address = x;
}

// public native float getFloat(long address);
static jfloat getFloat(JNIEnv *env, jref _this, jlong address)
{
    return *(jfloat *) (intptr_t) address;
}

// public native void putDouble(long address, double x);
static void putDouble(JNIEnv *env, jref _this, jlong address, jdouble x)
{
    *(jdouble *) (intptr_t) address = x;
}

// public native double getDouble(long address);
static jdouble getDouble(JNIEnv *env, jref _this, jlong address)
{
    return *(jdouble *) (intptr_t) address;
}

// public native void putAddress(long address, long x);
static void putAddress(JNIEnv *env, jref _this, jlong address, jlong x)
{
    putLong(env, _this, address, x);
}

// public native long getAddress(long address);
static jlong getAddress(JNIEnv *env, jref _this, jlong address)
{
    return getLong(env, _this, address);
}

static void *_getPoint(jref o, jlong offset)
{
    void *p = NULL;

    if (o == NULL) {
        p = (void *) (intptr_t) offset;
    } else {
        if (o != NULL) {
            // offset 在这里表示数组下标(index)
            p = array_index(o, offset);
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
static void setMemory(JNIEnv *env, jref _this, jref o, jlong offset, jlong bytes, jbyte value)
{
    void *p = _getPoint(o, offset);
    assert(p != NULL);
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
static void copyMemory(JNIEnv *env, jref _this, 
                    jref src_base, jlong src_offset, 
                    jref dest_base, jlong dest_offset, jlong bytes)
{
    void *src_p = _getPoint(src_base, src_offset);
    void *dest_p = _getPoint(dest_base, dest_offset);

    assert(src_p != NULL);
    assert(dest_p != NULL);
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
static jint getLoadAverage(JNIEnv *env, jref _this, jref loadavg, jint nelems)
{
    JVM_PANIC("getLoadAverage"); // todo
}

// (Ljava/lang/Class;)Z
static jboolean shouldBeInitialized(JNIEnv *env, jref _this, jclsref c)
{
    // todo
    return c->jvm_mirror->state >= CLASS_INITED ? jtrue : jfalse;
}

/**
 * Report the size in bytes of a native memory page (whatever that is).
 * This value will always be a power of two.
 * 
 * public native int pageSize();
*/
static jint _pageSize(JNIEnv *env, jref _this)
{
    return page_size();
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
static jclsref defineAnonymousClass(JNIEnv *env, jref _this, jclsref host_class, jref data, jref cp_patches)
{
    assert(host_class != NULL);
    assert(data != NULL && is_array_object(data));
    assert(cp_patches == NULL || is_array_object(cp_patches));

    Class *c = define_class(host_class->jvm_mirror->loader, (u1 *) data->data, data->arr_len);
    if (c == NULL)
        return NULL; // todo

    int cp_patches_len = cp_patches == NULL ? 0 : cp_patches->arr_len;
    for (int i = 0; i < cp_patches_len; i++) {
        jref o = array_get(jref, cp_patches, i);
        if (o != NULL) {
            u1 type = cp_get_type(&c->cp, i);
            if (type == JVM_CONSTANT_String) {
                cp_set(&c->cp, i, (slot_t) o);
                cp_set_type(&c->cp, i, JVM_CONSTANT_ResolvedString);
            } else {
                JVM_PANIC("defineAnonymousClass: unimplemented patch type"); // todo
            }
        }
    }

    c->nest_host = host_class->jvm_mirror;
    link_class(c);

    return c->java_mirror;
}

/** Lock the object.  It must get unlocked via {@link #monitorExit}. */
//  public native void monitorEnter(Object o);
static void monitorEnter(JNIEnv *env, jref _this, jref o)
{
    JVM_PANIC("monitorEnter"); // todo
}

/**
 * Unlock the object.  It must have been locked via monitorEnter.
 * 
 * public native void monitorExit(Object o);
 */
static void monitorExit(JNIEnv *env, jref _this, jref o)
{
    JVM_PANIC("monitorExit"); // todo
}

/**
  848        * Tries to lock the object.  Returns true or false to indicate
  849        * whether the lock succeeded.  If it did, the object must be
  850        * unlocked via {@link #monitorExit}.
  851        */
// public native boolean tryMonitorEnter(Object o);
static jboolean tryMonitorEnter(JNIEnv *env, jref _this, jref o)
{
    JVM_PANIC("tryMonitorEnter"); // todo
}

/** Throw the exception without telling the verifier. */
// public native void throwException(Throwable ee);
static void throwException(JNIEnv *env, jref _this, jref ee)
{
    JVM_PANIC("throwException"); // todo
}

// "()V"
static void loadFence(JNIEnv *env, jref _this)
{
    JVM_PANIC("loadFence"); // todo
}

// "()V"
static void storeFence(JNIEnv *env, jref _this)
{
//    printvm("storeFence\n");
    return; // todo
    JVM_PANIC("storeFence"); // todo
}

// "()V"
static void fullFence(JNIEnv *env, jref _this)
{
    JVM_PANIC("fullFence"); // todo
}

// ()Z
static jbool isBigEndian(JNIEnv *env)
{
    return is_big_endian() ? jtrue : jfalse;
}

#undef CLD
#define CLD "Ljava/lang/ClassLoader;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "park", "(ZJ)V", park },
        { "unpark", _OBJ_ "V", unpark },

        // compare and swap
        { "compareAndSwapInt", _OBJ "JII)Z", compareAndSwapInt },
        { "compareAndSwapLong", _OBJ "JJJ)Z", compareAndSwapLong },
        { "compareAndSwapObject", _OBJ "J" OBJ OBJ_ "Z", compareAndSwapObject },

        { "compareAndSetInt", _OBJ "JII)Z", compareAndSwapInt },
        { "compareAndSetLong", _OBJ "JJJ)Z", compareAndSwapLong },
        { "compareAndSetReference", _OBJ "J" OBJ OBJ_ "Z", compareAndSwapObject },

        // class
        { "allocateInstance", _CLS_ OBJ, allocateInstance },
        { "defineClass", _STR "[BII" CLD "Ljava/security/ProtectionDomain;)" CLS, defineClass },
        { "ensureClassInitialized", _CLS_ "V", ensureClassInitialized },
        { "ensureClassInitialized0", _CLS_ "V", ensureClassInitialized },
        { "staticFieldOffset", "(Ljava/lang/reflect/Field;)J", staticFieldOffset },
        { "staticFieldBase", "(Ljava/lang/reflect/Field;)" OBJ, staticFieldBase },

        // Object
        { "arrayBaseOffset", _CLS_"I", arrayBaseOffset },
        { "arrayBaseOffset0", _CLS_"I", arrayBaseOffset },
        { "arrayIndexScale", _CLS_"I", arrayIndexScale },
        { "arrayIndexScale0", _CLS_"I", arrayIndexScale },
        { "objectFieldOffset", "(Ljava/lang/reflect/Field;)J", objectFieldOffset },
        { "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", objectFieldOffset },
        { "objectFieldOffset1", _CLS STR_ "J", objectFieldOffset1 },

        { "getBoolean", _OBJ "J)Z", obj_getBoolean },
        { "putBoolean", _OBJ "JZ)V", obj_putBoolean },
        { "getByte", _OBJ "J)B", obj_getByte },
        { "putByte", _OBJ "JB)V", obj_putByte },
        { "getChar", _OBJ "J)C", obj_getChar },
        { "putChar", _OBJ "JC)V", obj_putChar },
        { "getShort", _OBJ "J)S", obj_getShort },
        { "putShort", _OBJ "JS)V", obj_putShort },
        { "getInt", _OBJ "J)I", obj_getInt },
        { "putInt", _OBJ "JI)V", obj_putInt },
        { "getLong", _OBJ "J)J", obj_getLong },
        { "putLong", _OBJ "JJ)V", obj_putLong },
        { "getFloat", _OBJ "J)F", obj_getFloat },
        { "putFloat", _OBJ "JF)V", obj_putFloat },
        { "getDouble", _OBJ "J)D", obj_getDouble },
        { "putDouble", _OBJ "JD)V", obj_putDouble },
        { "getObject", _OBJ "J)" OBJ, obj_getObject },
        { "getReference", _OBJ "J)" OBJ, obj_getObject },
        { "putObject", _OBJ "J" OBJ_ "V", obj_putObject },
        { "putReference", _OBJ "J" OBJ_ "V", obj_putObject },
        { "getOrderedObject", _OBJ "J)" OBJ, getOrderedObject },
        { "putOrderedObject", _OBJ "J" OBJ_ "V", putOrderedObject },
        { "putOrderedInt", _OBJ "JI)V", putOrderedInt },
        { "putOrderedLong", _OBJ "JJ)V", putOrderedLong },

        { "putIntVolatile", _OBJ "JI)V", putIntVolatile },
        { "putBooleanVolatile", _OBJ "JZ)V", putBooleanVolatile },
        { "putByteVolatile", _OBJ "JB)V", putByteVolatile },
        { "putShortVolatile", _OBJ "JS)V", putShortVolatile },
        { "putCharVolatile", _OBJ "JC)V", putCharVolatile },
        { "putLongVolatile", _OBJ "JJ)V", putLongVolatile },
        { "putFloatVolatile", _OBJ "JF)V", putFloatVolatile },
        { "putDoubleVolatile", _OBJ "JD)V", putDoubleVolatile },
        { "putObjectVolatile", _OBJ "J" OBJ_ "V", putObjectVolatile },
        { "putReferenceVolatile", _OBJ "J" OBJ_ "V", putObjectVolatile },

        { "getCharVolatile", _OBJ "J)C", getCharVolatile },
        { "getIntVolatile", _OBJ "J)I", getIntVolatile },
        { "getBooleanVolatile", _OBJ "J)Z", getBooleanVolatile },
        { "getByteVolatile", _OBJ "J)B", getByteVolatile },
        { "getShortVolatile", _OBJ "J)S", getShortVolatile },
        { "getLongVolatile", _OBJ "J)J", getLongVolatile },
        { "getFloatVolatile", _OBJ "J)F", getFloatVolatile },
        { "getDoubleVolatile", _OBJ "J)D", getDoubleVolatile },
        { "getObjectVolatile", _OBJ "J)" OBJ, getObjectVolatile },
        { "getReferenceVolatile", _OBJ "J)" OBJ, getObjectVolatile },

        // unsafe memory
        { "allocateMemory", "(J)J", allocateMemory },
        { "reallocateMemory", "(JJ)J", reallocateMemory },
        { "setMemory", _OBJ "JJB)V", setMemory },
        { "copyMemory", _OBJ "JLjava/lang/Object;JJ)V", copyMemory },
        { "freeMemory", "(J)V", freeMemory },
        { "addressSize", "()I", addressSize },
        { "addressSize0", "()I", addressSize },
        { "putAddress", "(JJ)V", putAddress },
        { "getAddress", "(J)J", getAddress },
        { "putByte", "(JB)V", putByte },
        { "getByte", "(J)B", getByte },
        { "putShort", "(JS)V", putShort },
        { "getShort", "(J)S", getShort },
        { "putChar", "(JC)V", putChar },
        { "getChar", "(J)C", getChar },
        { "putInt", "(JI)V", putInt },
        { "getInt", "(J)I", getInt },
        { "putLong", "(JJ)V", putLong },
        { "getLong", "(J)J", getLong },
        { "putFloat", "(JF)V", putFloat },
        { "getFloat", "(J)F", getFloat },
        { "putDouble", "(JD)V", putDouble },
        { "getDouble", "(J)D", getDouble },

        { "shouldBeInitialized", _CLS_ "Z", shouldBeInitialized },
        { "shouldBeInitialized0", _CLS_ "Z", shouldBeInitialized },
        { "getLoadAverage", "([DI)I", getLoadAverage },
        { "pageSize", "()I", _pageSize },
        { "defineAnonymousClass", _CLS "[B[" OBJ_ CLS, defineAnonymousClass },
        { "monitorEnter", _OBJ_ "V", monitorEnter },
        { "monitorExit", _OBJ_ "V", monitorExit },
        { "tryMonitorEnter", _OBJ_ "Z", tryMonitorEnter },
        { "throwException", "(Ljava/lang/Throwable;)V", throwException },

        { "loadFence", "()V", loadFence },
        { "storeFence", "()V", storeFence },
        { "fullFence", "()V", fullFence },

        { "isBigEndian0", "()Z", isBigEndian },
        { NULL }
};

void sun_misc_Unsafe_registerNatives()
{
    if (IS_JDK9_PLUS) {
        register_natives("jdk/internal/misc/Unsafe", methods);
    } else {
        register_natives("sun/misc/Unsafe", methods);
    }
}
