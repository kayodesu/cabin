/*
 * Author: Jia Yang
 */

#include "../../registry.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/jobject.h"
#include "../../../util/bigendian.h"

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
    jint x = frame_locals_geti(frame, 5);

    jint value = slot_geti(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = islot(x);
        set_instance_field_value_by_id(o, offset, &s);
        frame_stack_pushi(frame, 1);
    } else {
        frame_stack_pushi(frame, 0);
    }
}

// public final native boolean compareAndSwapLong(Object o, long offset, long expected, long x);
static void compareAndSwapLong(struct frame *frame)
{
    //    jref this_obj = slot_getr(frame->local_vars);
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2); // long 占两个Slot
    jlong expected = frame_locals_getl(frame, 4);
    jlong x = frame_locals_getl(frame, 6);

    jlong value = slot_getl(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = lslot(x);
        set_instance_field_value_by_id(o, offset, &s);
        frame_stack_pushi(frame, 1);
    } else {
        frame_stack_pushi(frame, 0);
    }
}

// public final native boolean compareAndSwapObject(Object o, long offset, Object expected, Object x)
static void compareAndSwapObject(struct frame *frame)
{
//    jref this_obj = slot_getr(frame->local_vars);
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2); // long 占两个Slot
    jref expected = frame_locals_getr(frame, 4);
    jref x = frame_locals_getr(frame, 5);

    jref value = slot_getr(get_instance_field_value_by_id(o, offset));
    if (value == expected) {
        struct slot s = rslot(x);
        set_instance_field_value_by_id(o, offset, &s);
        frame_stack_pushi(frame, 1);
    } else {
        frame_stack_pushi(frame, 0);
    }
}

/*************************************    class    ************************************/

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

    jref field_obj =frame_locals_getr(frame, 1);
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
    jref o = frame_locals_getr(frame, 1); // first argument
    jlong offset = frame_locals_getl(frame, 2);

    jint value;
    if (jclass_is_array(o->jclass)) {
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

// public native Object getObjectVolatile(Object o, long offset);
static void getObjectVolatile(struct frame *frame)
{
    jvm_abort("");
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
    register_native_method("sun/misc/Unsafe~getIntVolatile~(Ljava/lang/Object;J)I", getIntVolatile);
    register_native_method("sun/misc/Unsafe~getLongVolatile~(Ljava/lang/Object;J)J", getLongVolatile);
    register_native_method("sun/misc/Unsafe~getBooleanVolatile~(Ljava/lang/Object;J)Z", getBooleanVolatile);
    register_native_method("sun/misc/Unsafe~getByteVolatile~(Ljava/lang/Object;J)B", getByteVolatile);
    register_native_method("sun/misc/Unsafe~getCharVolatile~(Ljava/lang/Object;J)C", getCharVolatile);
    register_native_method("sun/misc/Unsafe~getShortVolatile~(Ljava/lang/Object;J)S", getShortVolatile);
    register_native_method("sun/misc/Unsafe~getFloatVolatile~(Ljava/lang/Object;J)F", getFloatVolatile);
    register_native_method("sun/misc/Unsafe~getDoubleVolatile~(Ljava/lang/Object;J)D", getDoubleVolatile);

    // unsafe memory
    register_native_method("sun/misc/Unsafe~allocateMemory~(J)J", allocateMemory);
    register_native_method("sun/misc/Unsafe~reallocateMemory~(JJ)J", reallocateMemory);
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
}

/*

struct mem_info {
    jlong start_add; // start address, include.
    jlong end_add;   // end address, exclude.
    struct mem_info *next;
    u1 mem[];
} head = {
    0, // head 不是有效的地址，表示空地址。 todo
    sizeof(jlong) * 8,
    NULL
};

static struct mem_info* __allocate(jlong bytes)
{
    struct mem_info *curr = &head;

    for (struct mem_info *next = curr->next; next != NULL; curr = next, next = curr->next) {
        if (next->start_add - curr->end_add >= bytes) { // 找到一个空位
            break;
        }
    }

    VM_MALLOC_EXT(struct mem_info, 1, bytes, info);
    info->start_add = curr->end_add;
    info->end_add = info->start_add + bytes;
    info->next = curr->next;
    curr->next = info;
    return info;
}

static inline struct mem_info* __find_add_prev(jlong address)
{
    for (struct mem_info *prev = &head, *curr = head.next; curr != NULL; prev = curr, curr = curr->next) {
        if (curr->start_add <= address && address < curr->end_add) {
            return prev;
        }
    }

    jvm_abort("invalid address: %ld\n", address); // todo
}

static inline void *__memory_at(jlong address)
{
    struct mem_info *info = __find_add_prev(address)->next;
    return info->mem + (address - info->start_add);
}

// free address
static inline void __free_add(jlong address)
{
    if (address != 0) {
        struct mem_info *prev = __find_add_prev(address);
        struct mem_info *curr = prev->next;
        prev->next = curr->next;
        free(curr);
    }
}

// public native long allocateMemory(long bytes);
static void allocateMemory(struct frame *frame)
{
    jlong bytes = slot_getl(frame->local_vars + 1);
    os_pushl(frame->operand_stack, __allocate(bytes)->start_add);
}

// public native long reallocateMemory(long address, long bytes);
static void reallocateMemory(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jlong bytes = (jbyte) slot_getl(frame->local_vars + 3);

    if (bytes == 0) {
        // 如果 bytes 为0，效果等同于 free
        __free_add(address);
        return;
    }

    if (address == 0) {
        // address == 0，等同于 malloc
        os_pushl(frame->operand_stack, __allocate(bytes)->start_add);
        return;
    }

    struct mem_info *prev = __find_add_prev(address);
    struct mem_info *curr = prev->next;
    if (curr->start_add != address) {
        jvm_abort("invalid address: %ld\n", address); // todo
    }
    if (curr->end_add - curr->start_add > bytes) {
        os_pushl(frame->operand_stack, address);
        return;
    }

    struct mem_info *next = curr->next;
    if (next == NULL || (next->start_add - curr->start_add >= bytes)) { // 后续空间足够
        struct mem_info *p = realloc(curr, (size_t) bytes);
        prev->next = p;
        p->next = next;
    } else { // 后续空间不够，从新申请一块新内存
        struct mem_info *info = __allocate(bytes);
        memcpy(info->mem, curr->mem, (size_t) (curr->end_add - curr->start_add));
        prev->next = curr->next;
        free(curr);
    }
}

// public native void freeMemory(long address);
static void freeMemory(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    __free_add(address);
}

// public native int addressSize();
static void addressSize(struct frame *frame)
{
    os_pushi(frame->operand_stack, sizeof(jlong)); // todo
}

// public native void putByte(long address, byte x);
static void putByte(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jbyte x = (jbyte) slot_geti(frame->local_vars + 3);
    *(jbyte *) __memory_at(address) = BIGENDIAN(x);
}

// public native byte getByte(long address);
static void getByte(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushi(frame->operand_stack, *(jbyte *) __memory_at(address));
}

// public native void putChar(long address, char x);
static void putChar(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jchar x = (jchar) slot_geti(frame->local_vars + 3);
    *(jchar *) __memory_at(address) = BIGENDIAN(x);
}

// public native char getChar(long address);
static void getChar(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushi(frame->operand_stack, *(jchar *) __memory_at(address));
}

// public native void putShort(long address, short x);
static void putShort(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jshort x = (jshort) slot_geti(frame->local_vars + 3);
    *(jshort *) __memory_at(address) = BIGENDIAN(x);
}

// public native short getShort(long address);
static void getShort(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushi(frame->operand_stack, *(jshort *) __memory_at(address));
}

// public native void putInt(long address, int x);
static void putInt(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jint x = slot_geti(frame->local_vars + 3);
    *(jint *) __memory_at(address) = BIGENDIAN(x);
}

// public native int getInt(long address);
static void getInt(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushi(frame->operand_stack, *(jint *) __memory_at(address));
}

// public native void putLong(long address, long x);
static void putLong(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jlong x = slot_getl(frame->local_vars + 3);
    *(jlong *) __memory_at(address) = BIGENDIAN(x);
}

// public native long getLong(long address);
static void getLong(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushl(frame->operand_stack, *(jlong *) __memory_at(address));
}

// public native void putFloat(long address, float x);
static void putFloat(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jfloat x = slot_getf(frame->local_vars + 3);
    *(jfloat *) __memory_at(address) = BIGENDIAN(x);
}

// public native float getFloat(long address);
static void getFloat(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushf(frame->operand_stack, *(jfloat *) __memory_at(address));
}

// public native void putDouble(long address, double x);
static void putDouble(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    jdouble x = slot_getd(frame->local_vars + 3);
    *(jdouble *) __memory_at(address) = BIGENDIAN(x);
}

// public native double getDouble(long address);
static void getDouble(struct frame *frame)
{
    jlong address = slot_getl(frame->local_vars + 1);
    os_pushd(frame->operand_stack, *(jdouble *) __memory_at(address));
}

 */
