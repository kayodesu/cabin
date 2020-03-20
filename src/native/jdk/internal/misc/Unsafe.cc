/*
 * Author: kayo
 */

#define sun_misc_Unsafe_registerNatives xxxffge
#include "../../../sun/misc/Unsafe.cc"

// private native long objectFieldOffset1(Class<?> c, String name);
static void objectFieldOffset1(Frame *frame)
{
    // todo
    jvm_abort("objectFieldOffset1");
}

void jdk_internal_misc_Unsafe_registerNatives()
{
#undef C
#define C "jdk/internal/misc/Unsafe"

#define LCLD "Ljava/lang/ClassLoader;"
    registerNative(C, "park", "(ZJ)V", park);
    registerNative(C, "unpark", "(Ljava/lang/Object;)V", unpark);

    // compare and swap
    registerNative(C, "compareAndSwapInt", "(Ljava/lang/Object;JII)Z", compareAndSwapInt);
    registerNative(C, "compareAndSwapLong", "(Ljava/lang/Object;JJJ)Z", compareAndSwapLong);
    registerNative(C, "compareAndSwapObject", "(" LOBJ "J" LOBJ LOBJ")Z", compareAndSwapObject);

    // class
    registerNative(C, "allocateInstance", "(Ljava/lang/Class;)Ljava/lang/Object;", allocateInstance);
    registerNative(C, "defineClass", "(" LSTR "[BII" LCLD "Ljava/security/ProtectionDomain;)" LCLS, defineClass);
    registerNative(C, "ensureClassInitialized0", "(Ljava/lang/Class;)V", ensureClassInitialized);
    registerNative(C, "staticFieldOffset0", "(Ljava/lang/reflect/Field;)J", staticFieldOffset);
    registerNative(C, "staticFieldBase0", "(Ljava/lang/reflect/Field;)" LOBJ, staticFieldBase);

    // Object
    registerNative(C, "arrayBaseOffset0", "(Ljava/lang/Class;)I", arrayBaseOffset);
    registerNative(C, "arrayIndexScale0", "(Ljava/lang/Class;)I", arrayIndexScale);
    registerNative(C, "objectFieldOffset0", "(Ljava/lang/reflect/Field;)J", objectFieldOffset);
    registerNative(C, "objectFieldOffset1", "(Ljava/lang/Class;Ljava/lang/String;)J", objectFieldOffset1);

    registerNative(C, "getBoolean", "(Ljava/lang/Object;J)Z", getBoolean);
    registerNative(C, "putBoolean", "(Ljava/lang/Object;JZ)V", putBoolean);
    registerNative(C, "getByte", "(Ljava/lang/Object;J)B", obj_getByte);
    registerNative(C, "putByte", "(Ljava/lang/Object;JB)V", obj_putByte);
    registerNative(C, "getChar", "(Ljava/lang/Object;J)C", obj_getChar);
    registerNative(C, "putChar", "(Ljava/lang/Object;JC)V", obj_putChar);
    registerNative(C, "getShort", "(Ljava/lang/Object;J)S", obj_getShort);
    registerNative(C, "putShort", "(Ljava/lang/Object;JS)V", obj_putShort);
    registerNative(C, "getInt", "(Ljava/lang/Object;J)I", obj_getInt);
    registerNative(C, "putInt", "(Ljava/lang/Object;JI)V", obj_putInt);
    registerNative(C, "getLong", "(Ljava/lang/Object;J)J", obj_getLong);
    registerNative(C, "putLong", "(Ljava/lang/Object;JJ)V", obj_putLong);
    registerNative(C, "getFloat", "(Ljava/lang/Object;J)F", obj_getFloat);
    registerNative(C, "putFloat", "(Ljava/lang/Object;JF)V", obj_putFloat);
    registerNative(C, "getDouble", "(Ljava/lang/Object;J)D", obj_getDouble);
    registerNative(C, "putDouble", "(Ljava/lang/Object;JD)V", obj_putDouble);
    registerNative(C, "getObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObject);
    registerNative(C, "putObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObject);
    registerNative(C, "getObjectVolatile", "(Ljava/lang/Object;J)Ljava/lang/Object;", getObjectVolatile);
    registerNative(C, "putObjectVolatile", "(Ljava/lang/Object;JLjava/lang/Object;)V", putObjectVolatile);
    registerNative(C, "getOrderedObject", "(Ljava/lang/Object;J)Ljava/lang/Object;", getOrderedObject);
    registerNative(C, "putOrderedObject", "(Ljava/lang/Object;JLjava/lang/Object;)V", putOrderedObject);
    registerNative(C, "putOrderedInt", "(Ljava/lang/Object;JI)V", putOrderedInt);
    registerNative(C, "putOrderedLong", "(Ljava/lang/Object;JJ)V", putOrderedLong);

    registerNative(C, "putIntVolatile", "(Ljava/lang/Object;JI)V", putIntVolatile);
    registerNative(C, "putBooleanVolatile", "(Ljava/lang/Object;JZ)V", putBooleanVolatile);
    registerNative(C, "putByteVolatile", "(Ljava/lang/Object;JB)V", putByteVolatile);
    registerNative(C, "putShortVolatile", "(Ljava/lang/Object;JS)V", putShortVolatile);
    registerNative(C, "putCharVolatile", "(Ljava/lang/Object;JC)V", putCharVolatile);
    registerNative(C, "putLongVolatile", "(Ljava/lang/Object;JJ)V", putLongVolatile);
    registerNative(C, "putFloatVolatile", "(Ljava/lang/Object;JF)V", putFloatVolatile);
    registerNative(C, "putDoubleVolatile", "(Ljava/lang/Object;JD)V", putDoubleVolatile);

    registerNative(C, "getIntVolatile", "(Ljava/lang/Object;J)I", getIntVolatile);
    registerNative(C, "getBooleanVolatile", "(Ljava/lang/Object;J)Z", getBooleanVolatile);
    registerNative(C, "getByteVolatile", "(Ljava/lang/Object;J)B", getByteVolatile);
    registerNative(C, "getShortVolatile", "(Ljava/lang/Object;J)S", getShortVolatile);
    registerNative(C, "getCharVolatile", "(Ljava/lang/Object;J)C,", getCharVolatile);
    registerNative(C, "getLongVolatile", "(Ljava/lang/Object;J)J", getLongVolatile);
    registerNative(C, "getFloatVolatile", "(Ljava/lang/Object;J)F", getFloatVolatile);
    registerNative(C, "getDoubleVolatile", "(Ljava/lang/Object;J)D", getDoubleVolatile);

    // unsafe memory
    registerNative(C, "allocateMemory0", "(J)J", allocateMemory);
    registerNative(C, "reallocateMemory0", "(JJ)J", reallocateMemory);
    registerNative(C, "setMemory0", "(Ljava/lang/Object;JJB)V", setMemory);
    registerNative(C, "copyMemory0", "(Ljava/lang/Object;JLjava/lang/Object;JJ)V", copyMemory);
    registerNative(C, "freeMemory0", "(J)V", freeMemory);
    registerNative(C, "addressSize", "()I", addressSize);
    registerNative(C, "putAddress", "(JJ)V", putAddress);
    registerNative(C, "getAddress", "(J)J", getAddress);
    registerNative(C, "putByte", "(JB)V", putByte);
    registerNative(C, "getByte", "(J)B", getByte);
    registerNative(C, "putShort", "(JS)V", putShort);
    registerNative(C, "getShort", "(J)S", getShort);
    registerNative(C, "putChar", "(JC)V", putChar);
    registerNative(C, "getChar", "(J)C", getChar);
    registerNative(C, "putInt", "(JI)V", putInt);
    registerNative(C, "getInt", "(J)I", getInt);
    registerNative(C, "putLong", "(JJ)V", putLong);
    registerNative(C, "getLong", "(J)J", getLong);
    registerNative(C, "putFloat", "(JF)V", putFloat);
    registerNative(C, "getFloat", "(J)F", getFloat);
    registerNative(C, "putDouble", "(JD)V", putDouble);
    registerNative(C, "getDouble", "(J)D", getDouble);

    registerNative(C, "shouldBeInitialized0", "(Ljava/lang/Class;)Z", shouldBeInitialized);
    registerNative(C, "getLoadAverage0", "([DI)I", getLoadAverage);
    registerNative(C, "pageSize", "()I", pageSize);
    registerNative(C, "defineAnonymousClass0", "(Ljava/lang/Class;[B[Ljava/lang/Object;)" LCLS, defineAnonymousClass);
    registerNative(C, "monitorEnter", "(Ljava/lang/Object;)V", monitorEnter);
    registerNative(C, "monitorExit", "(Ljava/lang/Object;)V", monitorExit);
    registerNative(C, "tryMonitorEnter", "(Ljava/lang/Object;)Z", tryMonitorEnter);
    registerNative(C, "throwException", "(Ljava/lang/Throwable;)V", throwException);

    registerNative(C, "loadFence", "()V", loadFence);
    registerNative(C, "storeFence", "()V", storeFence);
    registerNative(C, "fullFence", "()V", fullFence);
}
