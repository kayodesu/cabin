#include <assert.h>
#include "cabin.h"
#include "jni.h"
#include "util/hash.h"
#include "util/encoding.h"
#include "util/convert.h"
#include "util/endianness.h"


#define OBJ   "Ljava/lang/Object;"
#define _OBJ  "(Ljava/lang/Object;"
#define OBJ_  "Ljava/lang/Object;)"
#define _OBJ_ "(Ljava/lang/Object;)"
#define __OBJ "()Ljava/lang/Object;"

#define CLS   "Ljava/lang/Class;"
#define _CLS  "(Ljava/lang/Class;"
#define CLS_  "Ljava/lang/Class;)"
#define _CLS_ "(Ljava/lang/Class;)"
#define __CLS "()Ljava/lang/Class;"

#define STR   "Ljava/lang/String;"
#define _STR  "(Ljava/lang/String;"
#define STR_  "Ljava/lang/String;)"
#define _STR_ "(Ljava/lang/String;)"
#define __STR "()Ljava/lang/String;"

static inline void _registerNatives() { }

#define JNINativeMethod_registerNatives { "registerNatives", "()V", (void *) _registerNatives }

// public final native Class<?> getClass();
// static jobject getClass(JNIEnv *env, jobject this)
// {
//     if (this == NULL) {
//         // JNU_ThrowNullPointerException(env, NULL); todo
//         return NULL;
//     } else {
//         return (*env)->GetObjectClass(env, this);
//     }
// }

JNIEXPORT jint JNICALL JVM_IHashCode(JNIEnv *env, jobject obj);
JNIEXPORT jobject JNICALL JVM_Clone(JNIEnv *env, jobject _obj);
JNIEXPORT void JNICALL JVM_MonitorWait(JNIEnv *env, jobject obj, jlong ms);
JNIEXPORT void JNICALL JVM_MonitorNotify(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL JVM_MonitorNotifyAll(JNIEnv *env, jobject obj);

static JNINativeMethod Object_natives[] = {
        JNINativeMethod_registerNatives,
        { "hashCode", "()I", JVM_IHashCode },
        // { "getClass", __CLS, getClass },
        { "clone", __OBJ, JVM_Clone },
        { "notifyAll", "()V", JVM_MonitorNotifyAll },
        { "notify", "()V", JVM_MonitorNotify },
        { "wait", "(J)V", JVM_MonitorWait },
};

// /*
//  * VM support where maxDepth == -1 to request entire stack dump
//  *
//  * private static native ThreadInfo[] dumpThreads0(
//  *                      long[] ids, boolean lockedMonitors, boolean lockedSynchronizers, int maxDepth);
//  *
//  */
// static jref dumpThreads0(JNIEnv *env, jclass cls, jref _ids, jboolean lockedMonitors, jboolean lockedSynchronizers, jint maxDepth)
// {
//     Object *thread_infos;

//     Class *ac = loadArrayClass0("[Ljava/lang/management/ThreadInfo;");
//     if (_ids == NULL) { // dump all threads
//         thread_infos = alloc_array(ac, g_all_threads_count);

//         for (int i = 0; i < g_all_threads_count; i++) {
//             Thread *t = g_all_threads[i];
//             Object *thread_info = to_java_lang_management_ThreadInfo(t, lockedMonitors, lockedSynchronizers, maxDepth);
//             array_set_ref(thread_infos, i, thread_info);
//         }
//     } else {
//         jarrRef ids = (jarrRef)(_ids);
//         thread_infos = alloc_array(ac, ids->arr_len);

//         for (int i = 0; i < ids->arr_len; i++) {
//             jlong id = array_get(jlong, ids, i);
//             Thread *t = thread_from_id(id);
//             assert(t != NULL);
//             Object *thread_info = to_java_lang_management_ThreadInfo(t, lockedMonitors, lockedSynchronizers, maxDepth);
//             array_set_ref(thread_infos, i, thread_info);
//         }
//     }

//     return thread_infos;
// }

// static JNINativeMethod sun_management_ThreadImpl_NativeMethods[] = {
//         JNINativeMethod_registerNatives,
//         { "dumpThreads0", "([JZZ)[Ljava/lang/management/ThreadInfo;", dumpThreads0 },
// };

// native boolean closeScope0(Scope scope, Scope.ScopedAccessError exception);
static jbool closeScope0()
{
    // todo
    return true;
}

static JNINativeMethod ScopedMemoryAccess_natives[] = {
        JNINativeMethod_registerNatives,
        { "closeScope0", NULL, closeScope0 },
};

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

    JVM_PANIC("never go here"); // todo
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

#define MEM_SETTER_AND_GETTER(type) \
static void _mem_put_##type##_(JNIEnv *env, jref this, jlong address, j##type x) \
{ \
    *(j##type *) (intptr_t) address = x;  \
} \
 \
static j##type _mem_get_##type##_(JNIEnv *env, jref this, jlong address) \
{ \
    return *(j##type *) (intptr_t) address; \
}

MEM_SETTER_AND_GETTER(byte)
MEM_SETTER_AND_GETTER(char)
MEM_SETTER_AND_GETTER(short)
MEM_SETTER_AND_GETTER(int)
MEM_SETTER_AND_GETTER(long)
MEM_SETTER_AND_GETTER(float)
MEM_SETTER_AND_GETTER(double)

#undef MEM_SETTER_AND_GETTER


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
        JNINativeMethod_registerNatives,
        { "park", "(ZJ)V", park },
        { "unpark", _OBJ_ "V", unpark },

        // compare and swap
        { "compareAndSetInt", _OBJ "JII)Z", compareAndSwapInt },
        { "compareAndSetLong", _OBJ "JJJ)Z", compareAndSwapLong },
        { "compareAndSetReference", _OBJ "J" OBJ OBJ_ "Z", compareAndSwapObject },

        // class
        { "allocateInstance", _CLS_ OBJ, allocateInstance },
        { "defineClass", _STR "[BII" CLD "Ljava/security/ProtectionDomain;)" CLS, defineClass },
        { "ensureClassInitialized0", _CLS_ "V", ensureClassInitialized },
        { "staticFieldOffset", "(Ljava/lang/reflect/Field;)J", staticFieldOffset },
        { "staticFieldBase", "(Ljava/lang/reflect/Field;)" OBJ, staticFieldBase },

        // Object
        { "arrayBaseOffset0", _CLS_"I", arrayBaseOffset },
        { "arrayIndexScale0", _CLS_"I", arrayIndexScale },
        { "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", objectFieldOffset },
        { "objectFieldOffset1", _CLS STR_ "J", objectFieldOffset1 },

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
        { "getObject", _OBJ "J)" OBJ, _obj_get_ref_ },
        { "getReference", _OBJ "J)" OBJ, _obj_get_ref_ },
        { "putObject", _OBJ "J" OBJ_ "V", _obj_put_ref_ },
        { "putReference", _OBJ "J" OBJ_ "V", _obj_put_ref_ },

        { "putIntVolatile", _OBJ "JI)V", _obj_put_int_volatile },
        { "putBooleanVolatile", _OBJ "JZ)V", _obj_put_boolean_volatile },
        { "putByteVolatile", _OBJ "JB)V", _obj_put_byte_volatile },
        { "putShortVolatile", _OBJ "JS)V", _obj_put_short_volatile },
        { "putCharVolatile", _OBJ "JC)V", _obj_put_char_volatile },
        { "putLongVolatile", _OBJ "JJ)V", _obj_put_long_volatile },
        { "putFloatVolatile", _OBJ "JF)V", _obj_put_float_volatile },
        { "putDoubleVolatile", _OBJ "JD)V", _obj_put_double_volatile },
        { "putObjectVolatile", _OBJ "J" OBJ_ "V", putObjectVolatile },
        { "putReferenceVolatile", _OBJ "J" OBJ_ "V", putObjectVolatile },

        { "getCharVolatile", _OBJ "J)C", _obj_get_char_volatile },
        { "getIntVolatile", _OBJ "J)I", _obj_get_int_volatile },
        { "getBooleanVolatile", _OBJ "J)Z", _obj_get_boolean_volatile },
        { "getByteVolatile", _OBJ "J)B", _obj_get_byte_volatile },
        { "getShortVolatile", _OBJ "J)S", _obj_get_short_volatile },
        { "getLongVolatile", _OBJ "J)J", _obj_get_long_volatile },
        { "getFloatVolatile", _OBJ "J)F", _obj_get_float_volatile },
        { "getDoubleVolatile", _OBJ "J)D", _obj_get_double_volatile },
        { "getObjectVolatile", _OBJ "J)" OBJ,getObjectVolatile },
        { "getReferenceVolatile", _OBJ "J)" OBJ, getObjectVolatile },

        // unsafe memory
        { "allocateMemory0", "(J)J", allocateMemory },
        { "reallocateMemory0", "(JJ)J", reallocateMemory },
        { "setMemory0", _OBJ "JJB)V", setMemory },
        { "copyMemory0", _OBJ "JLjava/lang/Object;JJ)V", copyMemory },
        { "copySwapMemory0", _OBJ "JLjava/lang/Object;JJJ)V", copySwapMemory },
        { "freeMemory0", "(J)V", freeMemory },
        { "putByte", "(JB)V", _mem_put_byte_ },
        { "getByte", "(J)B", _mem_get_byte_ },
        { "putShort", "(JS)V", _mem_put_short_ },
        { "getShort", "(J)S", _mem_get_short_ },
        { "putChar", "(JC)V", _mem_put_char_ },
        { "getChar", "(J)C", _mem_get_char_ },
        { "putInt", "(JI)V", _mem_put_int_ },
        { "getInt", "(J)I", _mem_get_int_ },
        { "putLong", "(JJ)V", _mem_put_long_ },
        { "getLong", "(J)J", _mem_get_long_ },
        { "putFloat", "(JF)V", _mem_put_float_ },
        { "getFloat", "(J)F", _mem_get_float_ },
        { "putDouble", "(JD)V", _mem_put_double_ },
        { "getDouble", "(J)D", _mem_get_double_ },

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
;
    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
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

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);

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

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
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

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
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

static JNINativeMethod MethodHandle_natives[] = {
        JNINativeMethod_registerNatives,
        { "invokeExact",     "([" OBJ_ OBJ, MH_invokeExact },
        { "invoke",          "([" OBJ_ OBJ, MH_invoke },
        { "invokeBasic",     "([" OBJ_ OBJ, MH_invokeBasic },
        { "linkToVirtual",   "([" OBJ_ OBJ, MH_linkToVirtual },
        { "linkToStatic",    "([" OBJ_ OBJ, MH_linkToStatic },
        { "linkToSpecial",   "([" OBJ_ OBJ, MH_linkToSpecial },
        { "linkToInterface", "([" OBJ_ OBJ, MH_linkToInterface },
};


/*
 * Fetch MH-related JVM parameter.
 * which=0 retrieves MethodHandlePushLimit
 * which=1 retrieves stack slot push size (in address units)
 * 
 * static native int getConstant(int which);
 */
static jint MHN_getConstant(jint which)
{
    // todo 啥意思
    // if (which == 4)
    //     return 1;
    // else
        return 0;
}

/* Method flags */

#define MB_LAMBDA_HIDDEN        1
#define MB_LAMBDA_COMPILED      2
#define MB_CALLER_SENSITIVE     4
#define MB_DEFAULT_CONFLICT     8


#define IS_METHOD        0x010000
#define IS_CONSTRUCTOR   0x020000
#define IS_FIELD         0x040000
#define IS_TYPE          0x080000
#define CALLER_SENSITIVE 0x100000

#define SEARCH_SUPERCLASSES 0x100000
#define SEARCH_INTERFACES   0x200000

#define ALL_KINDS (IS_METHOD | IS_CONSTRUCTOR | IS_FIELD | IS_TYPE)
                
#define REFERENCE_KIND_SHIFT 24
#define REFERENCE_KIND_MASK  (0xf000000 >> REFERENCE_KIND_SHIFT)

static int __method_flags(Method *m) 
{
    assert(m != NULL);

    int flags = m->access_flags;

    if(m->access_flags & MB_CALLER_SENSITIVE)
        flags |= CALLER_SENSITIVE;

    return flags;
}

// static native void init(MemberName self, Object ref);
static void MHN_init(jref self, jref ref)
{
    initMemberName(self, ref);
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
static void MHN_expand(jref self)
{
    expandMemberName(self);
}

/*
 * static native MemberName resolve(MemberName self, Class<?> caller, int lookupMode,
            boolean speculativeResolve) throws LinkageError, ClassNotFoundException;
 */
static jref MHN_resolve(jref self/*MemberName*/, jclsRef caller, int lookupMode, jboolean speculativeResolve)
{
    // todo speculative_resolve
    return resolveMemberName(self, caller != NULL ? caller->jvm_mirror : NULL);
}

// static native int getMembers(Class<?> defc, String matchName, String matchSig,
//                              int matchFlags, Class<?> caller, int skip, MemberName[] results);
static jint MHN_getMembers(jclsRef defc, jstrRef match_name, jstrRef match_sig,
                           jint match_flags, jclsRef caller, jint skip, jref _results)
{
    assert(is_array_object(_results));
    jarrRef results = (jarrRef)(_results);
    int search_super = (match_flags & SEARCH_SUPERCLASSES) != 0;
    int search_intf = (match_flags & SEARCH_INTERFACES) != 0;
    int local = !(search_super || search_intf);
//    char *name_sym = NULL;
//    char *sig_sym = NULL;

    if (match_name != NULL) {
        // utf8_t *x = string_to_utf8(match_name);
        JVM_PANIC("unimplemented");
    }

    if (match_sig != NULL) {
        // utf8_t *x = string_to_utf8(match_sig);
        JVM_PANIC("unimplemented");
    }

    if(match_flags & IS_FIELD)
        JVM_PANIC("unimplemented");

    if(!local)
        JVM_PANIC("unimplemented");

    if(match_flags & (IS_METHOD | IS_CONSTRUCTOR)) {
        int count = 0;

        for (u2 i = 0; i < defc->jvm_mirror->methods_count; i++) {
            Method *m = defc->jvm_mirror->methods + i;
            if(m->name == SYMBOL(class_init))
                continue;
            if(m->name == SYMBOL(object_init))
                continue;
            if(skip-- > 0)
                continue;

            if(count < results->arr_len) {
                Object *member_name = array_get(jref, results, count);
                count++;
                int flags = __method_flags(m) | IS_METHOD;

                flags |= (IS_STATIC(m) ? JVM_REF_invokeStatic : JVM_REF_invokeVirtual) << REFERENCE_KIND_SHIFT;

                set_int_field(member_name, "flags", flags);
                set_ref_field(member_name, "clazz", "Ljava/lang/Class;", m->clazz->java_mirror);
                set_ref_field(member_name, "name", "Ljava/lang/String;", intern_string(alloc_string(m->name)));
                set_ref_field(member_name, "type", "Ljava/lang/Object;", alloc_string(m->descriptor));
                // INST_DATA(mname, int, mem_name_flags_offset) = flags;
                // INST_DATA(mname, Class*, mem_name_clazz_offset) = mb->class;
                // INST_DATA(mname, Object*, mem_name_name_offset) =
                //                 findInternedString(createString(mb->name));
                // INST_DATA(mname, Object*, mem_name_type_offset) =
                //                 createString(mb->type);
                // INST_DATA(mname, MethodBlock*, mem_name_vmtarget_offset) = mb;
            }
        }

        return count;
    }


    JVM_PANIC("unimplemented");
}

// static native long objectFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong MHN_objectFieldOffset(jref self)
{
    // private Class<?> clazz;       // class in which the method is defined
    // private String   name;        // may be null if not yet materialized
    // private Object   type;        // may be null if not yet materialized
    Class *clazz = get_ref_field(self, "clazz", "Ljava/lang/Class;")->jvm_mirror;
    jstrRef name = get_ref_field(self, "name", "Ljava/lang/String;");
    // type maybe a String or an Object[] or a MethodType
    // Object[]: (Class<?>) Object[0] is return type
    //           (Class<?>[]) Object[1] is parameter types
    jref type = get_ref_field(self, "type", "Ljava/lang/Object;");

    Method *m = lookup_inst_method(self->clazz, "getSignature", "()Ljava/lang/String;");
    jref sig = slot_get_ref(exec_java_func1(m, self));

    Field *f = lookup_field(clazz, string_to_utf8(name), string_to_utf8(sig));
    return f->id;
}

// static native long staticFieldOffset(MemberName self);  // e.g., returns vmindex
static jlong MHN_staticFieldOffset(jref self)
{
    JVM_PANIC("staticFieldOffset");
}

// static native Object staticFieldBase(MemberName self);  // e.g., returns clazz
static jref MHN_staticFieldBase(jref self)
{
    JVM_PANIC("staticFieldBase");
}

// static native Object getMemberVMInfo(MemberName self);  // returns {vmindex,vmtarget}
static jref MHN_getMemberVMInfo(jref self)
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
static void MHN_setCallSiteTargetNormal(jref site, jref target)
{
    JVM_PANIC("setCallSiteTargetNormal");
}

// static native void setCallSiteTargetVolatile(CallSite site, MethodHandle target);
static void MHN_setCallSiteTargetVolatile(jref site, jref target)
{
    JVM_PANIC("setCallSiteTargetVolatile");
}

#undef MM
#undef _MM_
#undef T

#define MM "Ljava/lang/invoke/MemberName;"
#define _MM_ "(Ljava/lang/invoke/MemberName;)"
#define T "(Ljava/lang/invoke/CallSite;Ljava/lang/invoke/MethodHandle)V"

static JNINativeMethod MethodHandleNatives_natives[] = {
    JNINativeMethod_registerNatives,

    // MemberName support

    {"init", "(" MM OBJ_ "V", MHN_init },
    {"expand", _MM_ "V", MHN_expand },
    {"resolve", "(" MM CLS "IZ)" MM, MHN_resolve },
    {"getMembers", _CLS STR STR "I" CLS "I[" MM ")I", MHN_getMembers },

    // Field layout queries parallel to sun.misc.Unsafe:

    {"objectFieldOffset", _MM_ "J", MHN_objectFieldOffset },
    {"staticFieldOffset", _MM_ "J", MHN_staticFieldOffset },
    {"staticFieldBase", _MM_ OBJ, MHN_staticFieldBase },
    {"getMemberVMInfo", _MM_ OBJ, MHN_getMemberVMInfo },

    // MethodHandle support
    {"getConstant", "(I)I", MHN_getConstant },

    // CallSite support
    /* Tell the JVM that we need to change the target of a CallSite. */
    {"setCallSiteTargetNormal", T, MHN_setCallSiteTargetNormal },
    {"setCallSiteTargetVolatile", T, MHN_setCallSiteTargetVolatile },
};

#undef MM
#undef _MM_
#undef T

void init_native() 
{
    register_natives("java/lang/Object", Object_natives, ARRAY_LENGTH(Object_natives));
    register_natives("jdk/internal/misc/ScopedMemoryAccess", 
                    ScopedMemoryAccess_natives, ARRAY_LENGTH(ScopedMemoryAccess_natives));
    register_natives("jdk/internal/misc/Unsafe", Unsafe_natives, ARRAY_LENGTH(Unsafe_natives));
    register_natives("java/lang/invoke/MethodHandle", MethodHandle_natives, ARRAY_LENGTH(MethodHandle_natives));
    register_natives("java/lang/invoke/MethodHandleNatives", MethodHandleNatives_natives, ARRAY_LENGTH(MethodHandleNatives_natives));
}


#define NATIVE_METHOD_OWNED_CLASS_MAX_COUNT 512 // big enough

static struct native_owned_class {
    const utf8_t *class_name;
    JNINativeMethod *methods;
    int methods_count;
} classes[NATIVE_METHOD_OWNED_CLASS_MAX_COUNT];

static int classes_count = 0;

void register_natives(const char *class_name, JNINativeMethod *methods, int methods_count)
{
    assert(class_name != NULL && methods != NULL && methods_count > 0);

    if (classes_count >= NATIVE_METHOD_OWNED_CLASS_MAX_COUNT) {
        ERR("Too many native methods, register natives failed"); // todo
        return;
    }

    // classes[classes_count].class_name = class_name;
    // classes[classes_count].methods = methods;
    // classes[classes_count].methods_count = methods_count;
    // classes_count++;
    classes[classes_count++] = (struct native_owned_class) { class_name, methods, methods_count };
}

JNINativeMethod *find_native_method(const char *class_name, const char *method_name, const char *method_descriptor)
{
    assert(class_name != NULL && method_name != NULL);

    for (int i = 0; i < classes_count; i++) {
        if (utf8_equals(classes[i].class_name, class_name)) {
            for (int j = 0; j < classes[i].methods_count; j++) {
                if (utf8_equals(classes[i].methods[j].name, method_name) 
                    && utf8_equals(classes[i].methods[j].signature, method_descriptor)) {
                    return &(classes[i].methods[j]);
                }
            }
            return NULL; // not find
        }
    }

    return NULL; // not find
}
