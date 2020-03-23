#include "../../../registry.h"
#include "../../../../kayo.h"
#include "../../../../runtime/frame.h"
#include "../../../../objects/array_object.h"

/*
 * Author: kayo
 */

// public static native Object get(Object array, int index)
//              throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void get(Frame *frame)
{
    jvm_abort("get");
}

// public static native void set(Object array, int index, Object value)
//               throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void set(Frame *frame)
{
    jvm_abort("set");
}

// public static native int getLength(Object array) throws IllegalArgumentException;
static void getLength(Frame *frame)
{
    jvm_abort("getLength");
}

// private static native Object newArray(Class<?> componentType, int length)
//                  throws NegativeArraySizeException;
static void newArray(Frame *frame)
{
    auto componentType = frame->getLocalAsRef<Class>(0);
    auto length = frame->getLocalAsInt(1);

    auto arr = newArray(componentType->arrayClass(), length);
    frame->pushr(arr);
}

void java_lang_reflect_Array_registerNatives()
{
#undef C
#define C "java/lang/reflect/Array"
    registerNative(C, "get", "(Ljava/lang/Object;I)Ljava/lang/Object;", get);
    registerNative(C, "set", "(Ljava/lang/Object;ILjava/lang/Object;)V", set);
    registerNative(C, "getLength", "(Ljava/lang/Object;)I", getLength);
    registerNative(C, "newArray", "(Ljava/lang/Class;I)Ljava/lang/Object;", newArray);
}