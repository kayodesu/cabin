#include <assert.h>
#include "cabin.h"
#include "jni.h"
#include "mh.h"
#include "object.h"
#include "symbol.h"
#include "interpreter.h"


#define OBJ   "Ljava/lang/Object;"
#define _OBJ  "(Ljava/lang/Object;"
#define OBJ_  "Ljava/lang/Object;)"

#define CLS   "Ljava/lang/Class;"
#define _CLS  "(Ljava/lang/Class;"
#define _CLS_ "(Ljava/lang/Class;)"

#define STR   "Ljava/lang/String;"

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
static void park(JNIEnv *env, jref this, jboolean isAbsolute, jlong time)
{
    JVM_PANIC("park");
}

// public native void unpark(Object thread);
static void unpark(JNIEnv *env, jref this, jref thread)
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

#define COMPARE_AND_SWAP(Type, jypte) \
static jboolean compareAndSwap##Type(JNIEnv *env, jref _this, jref o, jlong offset, jypte expected, jypte x) \
{ \
    jypte *old; \
    if (is_array_object(o)) { \
        old = (jypte *)(array_index(o, offset)); \
    } else { \
        assert(0 <= offset && offset < o->clazz->inst_fields_count); \
        old = (jypte *) (o->data + offset); \
    } \
 \
    bool b = __sync_bool_compare_and_swap(old, expected, x); \
    return b ? jtrue : jfalse; \
}

COMPARE_AND_SWAP(Int, jint)
COMPARE_AND_SWAP(Long, jlong)
COMPARE_AND_SWAP(Object, jref)

#undef COMPARE_AND_SWAP

/*************************************    class    ************************************/
/** Allocate an instance but do not run any constructor. Initializes the class if it has not yet been. */
// public native Object allocateInstance(Class<?> type) throws InstantiationException;
static jref allocateInstance(JNIEnv *env, jref _this, jclsRef type)
{
    JVM_PANIC("allocateInstance");
}

// public native Class defineClass(String name, byte[] b, int off, int len,
//                                  ClassLoader loader, ProtectionDomain protectionDomain)
static jclsRef defineClass(JNIEnv *env, jref _this, jstrRef name,
                           jref b, jint off, jint len, jref loader, jref protectionDomain)
{
    JVM_PANIC("define_class");
}

// public native void ensureClassInitialized(Class<?> c);
static void ensureClassInitialized(JNIEnv *env, jref _this, jclsRef c)
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

    SHOULD_NEVER_REACH_HERE("%s", name);
    return -1; 
}

// public native Object staticFieldBase(Field f);
static jref staticFieldBase(JNIEnv *env, jref _this, jref f)
{
    // private Class<?> clazz;
    jclsRef co = get_ref_field(f, "clazz", "Ljava/lang/Class;");
    return co;
}

/*************************************    object    ************************************/

// public native int arrayBaseOffset(Class<?> type)
static jint arrayBaseOffset(JNIEnv *env, jref _this, jclsRef type)
{
    return 0; // todo
}

// public native int arrayIndexScale(Class<?> type)
static jint arrayIndexScale(JNIEnv *env, jref _this, jclsRef type)
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
static jlong objectFieldOffset1(JNIEnv *env, jref _this, jclsRef c, jstrRef name)
{
    Field *f = get_declared_field(c->jvm_mirror, string_to_utf8(name));
    return f->id;
}

#define OBJ_SETTER_AND_GETTER(type, t) \
static j##type _obj_get_##type##_(JNIEnv *env, jref _this, jref o, jlong offset) \
{ \
    /* o == NULL 时get内存中的值， offset就是地址 */ \
    if (o == NULL) { \
        return *(j##type *) (intptr_t) offset; \
    } \
    if (is_array_object(o)) { /* get value from array */ \
        assert(0 <= offset && offset < o->arr_len); \
        return array_get(j##type, o, offset); \
    } else if (is_class_object(o)) { /* get static filed value */ \
        Class *c = o->jvm_mirror; \
        init_class(c);  \
        assert(0 <= offset && offset < c->fields_count); \
        Field *f = c->fields + offset; \
        return f->static_value.t; \
    } else { \
        assert(0 <= offset && offset < o->clazz->inst_fields_count); \
        return slot_get_##type(o->data + offset); \
    } \
} \
\
static void _obj_put_##type##_(JNIEnv *env, jref _this, jref o, jlong offset, j##type x) \
{ \
    /* o == NULL 时put值到内存中， offset就是地址 */ \
    if (o == NULL) { \
        *(j##type *) (intptr_t) offset = x; \
        return; \
    } \
    if (is_array_object(o)) { /* set value to array */ \
        assert(0 <= offset && offset < o->arr_len); \
        array_set_##type(o, offset, x); \
    } else if (is_class_object(o)) { /* set static filed value */ \
        Class *c = o->jvm_mirror; \
        init_class(c); \
        assert(0 <= offset && offset < c->fields_count); \
        Field *f = c->fields + offset; \
        f->static_value.t = x; \
    } else { \
        assert(0 <= offset && offset < o->clazz->inst_fields_count); \
        slot_set_##type(o->data + offset, x); \
    } \
}

OBJ_SETTER_AND_GETTER(boolean, z)
OBJ_SETTER_AND_GETTER(byte, b)
OBJ_SETTER_AND_GETTER(char, c)
OBJ_SETTER_AND_GETTER(short, s)
OBJ_SETTER_AND_GETTER(int, i)
OBJ_SETTER_AND_GETTER(long, j)
OBJ_SETTER_AND_GETTER(float, f)
OBJ_SETTER_AND_GETTER(double, d)
OBJ_SETTER_AND_GETTER(ref, r)

#undef OBJ_SETTER_AND_GETTER


#define OBJ_SETTER_AND_GETTER_VOLATILE(type) \
static j##type _obj_get_##type##_volatile(JNIEnv *env, jref this, jref o, jlong offset) \
{ \
    /* todo Volatile */ \
    return _obj_get_##type##_(env, this, o, offset);  \
} \
 \
static void _obj_put_##type##_volatile(JNIEnv *env, jref this, jref o, jlong offset, j##type x) \
{ \
    /* todo Volatile */ \
    _obj_put_##type##_(env, this, o, offset, x); \
}

OBJ_SETTER_AND_GETTER_VOLATILE(boolean)
OBJ_SETTER_AND_GETTER_VOLATILE(byte)
OBJ_SETTER_AND_GETTER_VOLATILE(char)
OBJ_SETTER_AND_GETTER_VOLATILE(short)
OBJ_SETTER_AND_GETTER_VOLATILE(int)
OBJ_SETTER_AND_GETTER_VOLATILE(long)
OBJ_SETTER_AND_GETTER_VOLATILE(float)
OBJ_SETTER_AND_GETTER_VOLATILE(double)

#undef OBJ_SETTER_AND_GETTER_VOLATILE

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

static void *_getPoint(jref o, jlong offset)
{
    void *p = NULL;

    if (o == NULL) {
        p = (void *) (intptr_t) offset;
    } else if (is_array_object(o)) {
        // offset 在这里表示数组下标(index)
        p = array_index(o, offset);
    } else {
        // offset 在这里表示 slot id.
        p = o->data;
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

// private native void copySwapMemory0(Object srcBase, long srcOffset, Object destBase, 
//                                long destOffset, long bytes, long elemSize);
static void copySwapMemory(JNIEnv *env, jref _this, 
                    jref src_base, jlong src_offset, 
                    jref dest_base, jlong dest_offset, jlong bytes, jlong elemSize)
{
    JVM_PANIC("copySwapMemory");
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
static jboolean shouldBeInitialized(JNIEnv *env, jref _this, jclsRef c)
{
    // todo
    return c->jvm_mirror->state >= CLASS_INITED ? jtrue : jfalse;
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
static jclsRef defineAnonymousClass(JNIEnv *env, jref _this, jclsRef host_class, jref data, jref cp_patches)
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

#undef CLD
#define CLD "Ljava/lang/ClassLoader;"

static JNINativeMethod Unsafe_natives[] = {
        { "park", "(ZJ)V", park },
        { "unpark", _OBJ ")V", unpark },

        // compare and swap
        { "compareAndSetInt", _OBJ "JII)Z", compareAndSwapInt },
        { "compareAndSetLong", _OBJ "JJJ)Z", compareAndSwapLong },
        { "compareAndSetReference", _OBJ "J" OBJ OBJ_ "Z", compareAndSwapObject },

        // class
        { "allocateInstance", _CLS_ OBJ, allocateInstance },
        { "defineClass0", "(" STR "[BII" CLD "Ljava/security/ProtectionDomain;)" CLS, defineClass },
        { "ensureClassInitialized0", _CLS_ "V", ensureClassInitialized },
        { "staticFieldOffset0", "(Ljava/lang/reflect/Field;)J", staticFieldOffset },
        { "staticFieldBase0", "(Ljava/lang/reflect/Field;)" OBJ, staticFieldBase },

        // Object
        { "arrayBaseOffset0", _CLS_"I", arrayBaseOffset },
        { "arrayIndexScale0", _CLS_"I", arrayIndexScale },
        { "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", objectFieldOffset },
        { "objectFieldOffset1", _CLS STR ")J", objectFieldOffset1 },

        { "getBoolean", _OBJ "J)Z", _obj_get_boolean_ },
        { "putBoolean", _OBJ "JZ)V", _obj_put_boolean_ },
        { "getByte", _OBJ "J)B", _obj_get_byte_ },
        { "putByte", _OBJ "JB)V", _obj_put_byte_ },
        { "getChar", _OBJ "J)C", _obj_get_char_ },
        { "putChar", _OBJ "JC)V", _obj_put_char_ },
        { "getShort", _OBJ "J)S", _obj_get_short_ },
        { "putShort", _OBJ "JS)V", _obj_put_short_ },
        { "getInt", _OBJ "J)I", _obj_get_int_ },
        { "putInt", _OBJ "JI)V", _obj_put_int_ },
        { "getLong", _OBJ "J)J", _obj_get_long_ },
        { "putLong", _OBJ "JJ)V", _obj_put_long_ },
        { "getFloat", _OBJ "J)F", _obj_get_float_ },
        { "putFloat", _OBJ "JF)V", _obj_put_float_ },
        { "getDouble", _OBJ "J)D", _obj_get_double_ },
        { "putDouble", _OBJ "JD)V", _obj_put_double_ },
        { "getReference", _OBJ "J)" OBJ, _obj_get_ref_ },
        { "putReference", _OBJ "J" OBJ_ "V", _obj_put_ref_ },

        { "putIntVolatile", _OBJ "JI)V", _obj_put_int_volatile },
        { "putBooleanVolatile", _OBJ "JZ)V", _obj_put_boolean_volatile },
        { "putByteVolatile", _OBJ "JB)V", _obj_put_byte_volatile },
        { "putShortVolatile", _OBJ "JS)V", _obj_put_short_volatile },
        { "putCharVolatile", _OBJ "JC)V", _obj_put_char_volatile },
        { "putLongVolatile", _OBJ "JJ)V", _obj_put_long_volatile },
        { "putFloatVolatile", _OBJ "JF)V", _obj_put_float_volatile },
        { "putDoubleVolatile", _OBJ "JD)V", _obj_put_double_volatile },
        { "putReferenceVolatile", _OBJ "J" OBJ_ "V", putObjectVolatile },

        { "getCharVolatile", _OBJ "J)C", _obj_get_char_volatile },
        { "getIntVolatile", _OBJ "J)I", _obj_get_int_volatile },
        { "getBooleanVolatile", _OBJ "J)Z", _obj_get_boolean_volatile },
        { "getByteVolatile", _OBJ "J)B", _obj_get_byte_volatile },
        { "getShortVolatile", _OBJ "J)S", _obj_get_short_volatile },
        { "getLongVolatile", _OBJ "J)J", _obj_get_long_volatile },
        { "getFloatVolatile", _OBJ "J)F", _obj_get_float_volatile },
        { "getDoubleVolatile", _OBJ "J)D", _obj_get_double_volatile },
        { "getReferenceVolatile", _OBJ "J)" OBJ, getObjectVolatile },

        // unsafe memory
        { "allocateMemory0", "(J)J", allocateMemory },
        { "reallocateMemory0", "(JJ)J", reallocateMemory },
        { "setMemory0", _OBJ "JJB)V", setMemory },
        { "copyMemory0", _OBJ "JLjava/lang/Object;JJ)V", copyMemory },
        { "copySwapMemory0", _OBJ "JLjava/lang/Object;JJJ)V", copySwapMemory },
        { "freeMemory0", "(J)V", freeMemory },

        { "shouldBeInitialized0", _CLS_ "Z", shouldBeInitialized },
        { "getLoadAverage0", "([DI)I", getLoadAverage },
        { "defineAnonymousClass0", _CLS "[B[" OBJ_ CLS, defineAnonymousClass },
        { "throwException", "(Ljava/lang/Throwable;)V", throwException },

        { "loadFence", "()V", loadFence },
        { "storeFence", "()V", storeFence },
        { "fullFence", "()V", fullFence },
};

/**
 * Invokes the method handle, allowing any caller type type, but requiring an exact type match.
 * The symbolic type type at the call site of {@code invokeExact} must
 * exactly match this method handle's {@link #type type}.
 * No conversions are allowed on arguments or return values.
 * <p>
 * When this method is observed via the Core Reflection API,
 * it will appear as a single native method, taking an object array and returning an object.
 * If this native method is invoked directly via
 * {@link java.lang.reflect.Method#invoke java.lang.reflect.Method.invoke}, via JNI,
 * or indirectly via {@link java.lang.invoke.MethodHandles.Lookup#unreflect Lookup.unreflect},
 * it will throw an {@code UnsupportedOperationException}.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 * @throws WrongMethodTypeException if the target's type is not identical with the caller's symbolic type type
 * @throws Throwable anything thrown by the underlying method propagates unchanged through the method handle call
 */
// public final native @PolymorphicSignature Object invokeExact(Object... args) throws Throwable;
static jref MH_invokeExact(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle

    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    return exec_java_r(target, args);
}

/**
 * Invokes the method handle, allowing any caller type type,
 * and optionally performing conversions on arguments and return values.
 * <p>
 * If the call site's symbolic type type exactly matches this method handle's {@link #type type},
 * the call proceeds as if by {@link #invokeExact invokeExact}.
 * <p>
 * Otherwise, the call proceeds as if this method handle were first
 * adjusted by calling {@link #asType asType} to adjust this method handle
 * to the required type, and then the call proceeds as if by
 * {@link #invokeExact invokeExact} on the adjusted method handle.
 * <p>
 * There is no guarantee that the {@code asType} call is actually made.
 * If the JVM can predict the results of making the call, it may perform
 * adaptations directly on the caller's arguments,
 * and call the target method handle according to its own exact type.
 * <p>
 * The resolved type type at the call site of {@code invoke} must
 * be a valid argument to the receivers {@code asType} method.
 * In particular, the caller must specify the same argument arity
 * as the callee's type,
 * if the callee is not a {@linkplain #asVarargsCollector variable arity collector}.
 * <p>
 * When this method is observed via the Core Reflection API,
 * it will appear as a single native method, taking an object array and returning an object.
 * If this native method is invoked directly via
 * {@link java.lang.reflect.Method#invoke java.lang.reflect.Method.invoke}, via JNI,
 * or indirectly via {@link java.lang.invoke.MethodHandles.Lookup#unreflect Lookup.unreflect},
 * it will throw an {@code UnsupportedOperationException}.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 * @throws WrongMethodTypeException if the target's type cannot be adjusted to the caller's symbolic type type
 * @throws ClassCastException if the target's type can be adjusted to the caller, but a reference cast fails
 * @throws Throwable anything thrown by the underlying method propagates unchanged through the method handle call
 */
// public final native @PolymorphicSignature Object invoke(Object... args) throws Throwable;
static jref MH_invoke(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle

    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    return exec_java_r(target, args);

#if 0
    // MemberName internalMemberName();
    Method *m = _this->clazz->lookupInstMethod("internalMemberName", "()Ljava/lang/invoke/MemberName;");
    jref member_name = getRef(execJavaFunc(m, {_this}));
    assert(member_name != NULL);
    // private Class<?> clazz;       // class in which the method is defined
    // private String   name;        // may be null if not yet materialized
    // private Object   type;        // may be null if not yet materialized
    Class *c = member_name->getRefField<ClsObj>(S(clazz), S(sig_java_lang_Class))->jvm_mirror;
    auto name = member_name->getRefField(S(name), S(sig_java_lang_String))->toUtf8();

    // public MethodType getInvocationType()
    m = member_name->clazz->lookupInstMethod("getInvocationType", "()Ljava/lang/invoke/MethodType;");
    jref method_type = getRef(execJavaFunc(m, {member_name}));
    string desc = unparseMethodDescriptor(method_type);
    // todo 判断desc，如果有基本类型参数，则参数值要进行unbox。

//    int num = numElementsInMethodDescriptor(desc.c_str());
//    slot_t args[num];
//
//    va_list ap;
//    va_start(ap, _this);
//    for (int i = 0; i < num; i++) {
//        Object *o = va_arg(ap, Object *);
//        args[i] = rslot(o);
//    }
//    va_end(ap);

    m = c->lookupMethod(name, desc.c_str());
    slot_t *slot = execJavaFunc(m, args);
    return getRef(slot);
#endif
}
//static jref invoke(jref _this, ...)
//{
//    assert(_this != NULL);
//    // _this is a object of subclass of java.lang.invoke.MethodHandle
//
//    // MemberName internalMemberName();
//    Method *m = _this->clazz->lookupInstMethod("internalMemberName", "()Ljava/lang/invoke/MemberName;");
//    jref member_name = getRef(execJavaFunc(m, {_this}));
//    // private Class<?> clazz;       // class in which the method is defined
//    // private String   name;        // may be null if not yet materialized
//    // private Object   type;        // may be null if not yet materialized
//    Class *c = member_name->getRefField<ClsObj>(S(clazz), S(sig_java_lang_Class))->jvm_mirror;
//    auto name = member_name->getRefField(S(name), S(sig_java_lang_String))->toUtf8();
//
//    // public MethodType getInvocationType()
//    m = member_name->clazz->lookupInstMethod("getInvocationType", "()Ljava/lang/invoke/MethodType;");
//    jref method_type = getRef(execJavaFunc(m, {member_name}));
//    string desc = unparseMethodDescriptor(method_type);
//    // todo 判断desc，如果有基本类型参数，则参数值要进行unbox。
//
//    int num = numElementsInMethodDescriptor(desc.c_str());
//    slot_t args[num];
//
//    va_list ap;
//    va_start(ap, _this);
//    for (int i = 0; i < num; i++) {
//        Object *o = va_arg(ap, Object *);
//        args[i] = rslot(o);
//    }
//    va_end(ap);
//
//    m = c->lookupMethod(name, desc.c_str());
//    slot_t *slot = execJavaFunc(m, args);
//    return getRef(slot);
//}

/**
 * Private method for trusted invocation of a method handle respecting simplified signatures.
 * Type mismatches will not throw {@code WrongMethodTypeException}, but could crash the JVM.
 * <p>
 * The caller signature is restricted to the following basic types:
 * Object, int, long, float, double, and void return.
 * <p>
 * The caller is responsible for maintaining type correctness by ensuring
 * that the each outgoing argument value is a member of the range of the corresponding
 * callee argument type.
 * (The caller should therefore issue appropriate casts and integer narrowing
 * operations on outgoing argument values.)
 * The caller can assume that the incoming result value is part of the range
 * of the callee's return type.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// final native @PolymorphicSignature Object invokeBasic(Object... args) throws Throwable;
static jref MH_invokeBasic(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle

    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    return exec_java_r(target, args);
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeVirtual}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToVirtual(Object... args) throws Throwable;
static jref MH_linkToVirtual(u2 args_slots_count, const slot_t *args)
{
    JVM_PANIC("linkToVirtual");
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeStatic}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToStatic(Object... args) throws Throwable;
static jref MH_linkToStatic(u2 args_slots_count, const slot_t *args)
{
    Object *member_name = slot_get_ref(args + args_slots_count - 1);
    Method *target = (Method *) (void *) get_ref_field(member_name, S(vmtarget), S(sig_java_lang_Object));
    assert(target != NULL);

    return exec_java_r(target, args);
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeSpecial}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToSpecial(Object... args) throws Throwable;
static jref MH_linkToSpecial(u2 args_slots_count, const slot_t *args)
{
    JVM_PANIC("linkToSpecial");
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeInterface}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToInterface(Object... args) throws Throwable;
static jref MH_linkToInterface(jref args)
{
    JVM_PANIC("linkToInterface");
}

// static native void init(MemberName self, Object ref);
static void MHN_init(JNIEnv *env, jclsRef cls, jref self, jref ref)
{
    init_member_name(self, ref);
}
/*
func getMNFlags(method *heap.Method) int32 {
	flags := int32(method.AccessFlags)
	if method.IsStatic() {
		flags |= MN_IS_METHOD | (references.RefInvokeStatic << MN_REFERENCE_KIND_SHIFT)
	} else if method.IsConstructor() {
		flags |= MN_IS_CONSTRUCTOR | (references.RefInvokeSpecial << MN_REFERENCE_KIND_SHIFT)
	} else {
		flags |= MN_IS_METHOD | (references.RefInvokeSpecial << MN_REFERENCE_KIND_SHIFT)
	}
	return flags
}
*/

// static native void expand(MemberName self);
static void MHN_expand(JNIEnv *env, jclsRef cls, jref self)
{
    expand_member_name(self);
}

/*
 * static native MemberName resolve(MemberName self, Class<?> caller, int lookupMode,
            boolean speculativeResolve) throws LinkageError, ClassNotFoundException;
 */
static jref MHN_resolve(JNIEnv *env, jclsRef cls, jref self/*MemberName*/, 
                        jclsRef caller, int lookupMode, jboolean speculativeResolve)
{
    // todo speculative_resolve
    return resolve_member_name(self, caller != NULL ? caller->jvm_mirror : NULL);
}

// static native int getMembers(Class<?> defc, String matchName, String matchSig,
//                              int matchFlags, Class<?> caller, int skip, MemberName[] results);
static jint MHN_getMembers(JNIEnv *env, jclsRef cls, jclsRef defc, jstrRef match_name, 
                        jstrRef match_sig, jint match_flags, jclsRef caller, jint skip, jref _results)
{
    return get_members(defc, match_name, match_sig, match_flags, caller, skip, _results);
}

// static native long objectFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong MHN_objectFieldOffset(JNIEnv *env, jclsRef cls, jref self)
{
    return member_name_object_field_offset(self);
}

// static native long staticFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong MHN_staticFieldOffset(JNIEnv *env, jclsRef cls, jref self)
{
    JVM_PANIC("staticFieldOffset");
}

// static native Object staticFieldBase(MemberName self);  // e.g., returns clazz
static jref MHN_staticFieldBase(JNIEnv *env, jclsRef cls, jref self)
{
    JVM_PANIC("staticFieldBase");
}

// static native Object getMemberVMInfo(MemberName self);  // returns {vmindex,vmtarget}
static jref MHN_getMemberVMInfo(JNIEnv *env, jclsRef cls, jref self)
{
    /*
     * return Object[2];
     *
     * 使用实例：
     *  Object vminfo = MethodHandleNatives.getMemberVMInfo(this); // this is a MemberName
        assert(vminfo instanceof Object[]);
        long vmindex = (Long) ((Object[])vminfo)[0];
        Object vmtarget = ((Object[])vminfo)[1];
     */
    JVM_PANIC("getMemberVMInfo");
}

// static native void setCallSiteTargetNormal(CallSite site, MethodHandle target);
static void MHN_setCallSiteTargetNormal(JNIEnv *env, jclsRef cls, jref site, jref target)
{
    JVM_PANIC("setCallSiteTargetNormal");
}

// static native void setCallSiteTargetVolatile(CallSite site, MethodHandle target);
static void MHN_setCallSiteTargetVolatile(JNIEnv *env, jclsRef cls, jref site, jref target)
{
    JVM_PANIC("setCallSiteTargetVolatile");
}

#undef MN
#undef _MN_
#define MN "Ljava/lang/invoke/MemberName;"
#define _MN_ "(Ljava/lang/invoke/MemberName;)"

static JNINativeMethod MethodHandleNatives_natives[] = {
    // MemberName support

    {"init", "(" MN OBJ_ "V", MHN_init},
    {"expand", _MN_ "V", MHN_expand},
    {"resolve", "(" MN CLS "IZ)" MN, MHN_resolve},
    {"getMembers", _CLS STR STR "I" CLS "I[" MN ")I", MHN_getMembers},

    // Field layout queries parallel to sun.misc.Unsafe:

    {"objectFieldOffset", _MN_ "J", MHN_objectFieldOffset},
    {"staticFieldOffset", _MN_ "J", MHN_staticFieldOffset},
    {"staticFieldBase", _MN_ OBJ, MHN_staticFieldBase},
    {"getMemberVMInfo", _MN_ OBJ, MHN_getMemberVMInfo},

    // CallSite support
    // Tell the JVM that we need to change the target of a CallSite.

    {"setCallSiteTargetNormal",
     "(Ljava/lang/invoke/CallSite;Ljava/lang/invoke/MethodHandle;)V", MHN_setCallSiteTargetNormal},
    {"setCallSiteTargetVolatile",
     "(Ljava/lang/invoke/CallSite;Ljava/lang/invoke/MethodHandle;)V", MHN_setCallSiteTargetVolatile},
};

// native boolean closeScope0(Scope scope, Scope.ScopedAccessError exception);
static jbool closeScope0(JNIEnv *env, jobject this, jobject scope, jobject exception)
{
    // todo 
    return true;
}

static JNINativeMethod ScopedMemoryAccess_natives[] = {
        { "closeScope0", "(Ljdk/internal/misc/ScopedMemoryAccess$Scope;"
                         "Ljdk/internal/misc/ScopedMemoryAccess$Scope$ScopedAccessError;)Z", 
                         closeScope0 },
};

JNIEXPORT jint JNICALL JVM_IHashCode(JNIEnv *env, jobject obj);
JNIEXPORT jobject JNICALL JVM_Clone(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL JVM_MonitorWait(JNIEnv *env, jobject obj, jlong ms);
JNIEXPORT void JNICALL JVM_MonitorNotify(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL JVM_MonitorNotifyAll(JNIEnv *env, jobject obj);

static void Unsafe_registerNatives(JNIEnv *env, jclass cls)
{
    (*env)->RegisterNatives(env, cls, Unsafe_natives, ARRAY_LENGTH(Unsafe_natives));
}

static void MethodHandleNatives_registerNatives(JNIEnv *env, jclass cls)
{
    (*env)->RegisterNatives(env, cls, 
                MethodHandleNatives_natives, ARRAY_LENGTH(MethodHandleNatives_natives));
}

static void ScopedMemoryAccess_registerNatives(JNIEnv *env, jclass cls)
{
    (*env)->RegisterNatives(env, cls, 
                ScopedMemoryAccess_natives, ARRAY_LENGTH(ScopedMemoryAccess_natives));
}

void init_native() 
{
    Class *c = load_boot_class(S(java_lang_Object));
    get_declared_method(c, "hashCode", "()I")->native_method = JVM_IHashCode;
    get_declared_method(c, "clone", "()" OBJ)->native_method = JVM_Clone;
    get_declared_method(c, "notifyAll", "()V")->native_method = JVM_MonitorNotifyAll;
    get_declared_method(c, "notify", "()V")->native_method = JVM_MonitorNotify;
    get_declared_method(c, "wait", "(J)V")->native_method = JVM_MonitorWait;

    c = load_boot_class("jdk/internal/misc/Unsafe");
    get_declared_method(c, "registerNatives", "()V")->native_method = Unsafe_registerNatives;

    c = load_boot_class("java/lang/invoke/MethodHandle");
    get_declared_method(c, "invokeExact",     "([" OBJ_ OBJ)->native_method = MH_invokeExact;
    get_declared_method(c, "invoke",          "([" OBJ_ OBJ)->native_method = MH_invoke;
    get_declared_method(c, "invokeBasic",     "([" OBJ_ OBJ)->native_method = MH_invokeBasic;
    get_declared_method(c, "linkToVirtual",   "([" OBJ_ OBJ)->native_method = MH_linkToVirtual;
    get_declared_method(c, "linkToStatic",    "([" OBJ_ OBJ)->native_method = MH_linkToStatic;
    get_declared_method(c, "linkToSpecial",   "([" OBJ_ OBJ)->native_method = MH_linkToSpecial;
    get_declared_method(c, "linkToInterface", "([" OBJ_ OBJ)->native_method = MH_linkToInterface;

    c = load_boot_class("java/lang/invoke/MethodHandleNatives");
    get_declared_method(c, "registerNatives", "()V")->native_method = MethodHandleNatives_registerNatives;

    c = load_boot_class("jdk/internal/misc/ScopedMemoryAccess");
    get_declared_method(c, "registerNatives", "()V")->native_method = ScopedMemoryAccess_registerNatives;
}
