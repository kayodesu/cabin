#include "../../../jni_inner.h"
#include "../../../../vmdef.h"
#include "../../../../objects/class.h"
#include "../../../../objects/array_object.h"

/*
 * Author: Yo Ka
 */

// public static native Object get(Object array, int index)
//              throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static jref get(jclsref clazz, jref array, jint index)
{
    jvm_abort("get");
}

// public static native void set(Object array, int index, Object value)
//               throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void set(jclsref clazz, jref array, jint index, jref value)
{
    jvm_abort("set");
}

// public static native int getLength(Object array) throws IllegalArgumentException;
static jint getLength(jclsref clazz, jref array)
{
    jvm_abort("getLength");
}

// private static native Object newArray(Class<?> componentType, int length)
//                  throws NegativeArraySizeException;
static jref __newArray(jclsref clazz, jclsref componentType, jint length)
{
    return newArray(componentType->arrayClass(), length);
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