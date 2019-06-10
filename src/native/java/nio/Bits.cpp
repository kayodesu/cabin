/*
 * Author: Jia Yang
 */

#include "../../../jvm.h"
#include "../../registry.h"

// static native void copyFromShortArray(Object src, long srcPos, long dstAddr, long length);
static void copyFromShortArray(Frame *frame)
{
    jvm_abort("copyFromShortArray"); // todo
}

// static native void copyToShortArray(long srcAddr, Object dst, long dstPos, long length);
static void copyToShortArray(Frame *frame)
{
    jvm_abort("copyToShortArray"); // todo
}

// static native void copyFromIntArray(Object src, long srcPos, long dstAddr, long length);
static void copyFromIntArray(Frame *frame)
{
    jvm_abort("copyFromIntArray"); // todo
}


// static native void copyToIntArray(long srcAddr, Object dst, long dstPos, long length);
static void copyToIntArray(Frame *frame)
{
    jvm_abort("copyToIntArray"); // todo
}

// static native void copyFromLongArray(Object src, long srcPos, long dstAddr, long length);
static void copyFromLongArray(Frame *frame)
{
    jvm_abort("copyFromLongArray"); // todo
}

// static native void copyToLongArray(long srcAddr, Object dst, long dstPos, long length);
static void copyToLongArray(Frame *frame)
{
    jvm_abort("copyToLongArray"); // todo
}


void java_nio_Bits_registerNatives()
{
    register_native_method("java/nio/Bits", "copyFromShortArray", "(Ljava/lang/Object;JJJ)V", copyFromShortArray);
    register_native_method("java/nio/Bits", "copyToShortArray", "(JLjava/lang/Object;JJ)V", copyToShortArray);
    register_native_method("java/nio/Bits", "copyFromIntArray", "(Ljava/lang/Object;JJJ)V", copyFromIntArray);
    register_native_method("java/nio/Bits", "copyToIntArray", "(JLjava/lang/Object;JJ)V", copyToIntArray);
    register_native_method("java/nio/Bits", "copyFromLongArray", "(Ljava/lang/Object;JJJ)V", copyFromLongArray);
    register_native_method("java/nio/Bits", "copyToLongArray", "(JLjava/lang/Object;JJ)V", copyToLongArray);
}
