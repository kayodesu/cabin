#include "../../../jni_inner.h"
#include "../../../../runtime/frame.h"
#include "../../../../objects/array_object.h"

/*
 * Author: Yo Ka
 */

// public static native Object get(Object array, int index)
//              throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static jobject get(JNIEnv *env, jclass clazz, jobject array, jint index)
{
    jvm_abort("get");
}

// public static native void set(Object array, int index, Object value)
//               throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void set(JNIEnv *env, jclass clazz, jobject array, jint index, jobject value)
{
    jvm_abort("set");
}

// public static native int getLength(Object array) throws IllegalArgumentException;
static jint getLength(JNIEnv *env, jclass clazz, jobject array)
{
    jvm_abort("getLength");
}

// private static native Object newArray(Class<?> componentType, int length)
//                  throws NegativeArraySizeException;
static jref __newArray(JNIEnv *env, jclass clazz, jclsref componentType, jint length)
{
//    jclass arrCls = cli_arrayClass(componentType);
//    env->NewObjectArray()

    auto arr = newArray(componentType->arrayClass(), length);
    return arr;
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