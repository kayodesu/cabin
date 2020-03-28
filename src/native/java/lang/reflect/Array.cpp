#include "../../../jni_inner.h"
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
static void __newArray(Frame *frame)
{
    auto componentType = frame->getLocalAsRef<Class>(0);
    auto length = frame->getLocalAsInt(1);

    auto arr = newArray(componentType->arrayClass(), length);
    frame->pushr(arr);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "get", "(Ljava/lang/Object;I)Ljava/lang/Object;", (void *) get },
        { "set", "(Ljava/lang/Object;ILjava/lang/Object;)V", (void *) set },
        { "getLength", "(Ljava/lang/Object;)I", (void *) getLength },
        { "newArray", "(Ljava/lang/Class;I)" OBJ, (void *) __newArray },
};

void java_lang_reflect_Array_registerNatives()
{
    registerNatives("java/lang/reflect/Array", methods, ARRAY_LENGTH(methods));
}