#include "../../../jni_internal.h"
#include "../../../../objects/array.h"
#include "../../../../objects/prims.h"
#include "../../../../metadata/class.h"
#include "../../../../runtime/vm_thread.h"
#include "../../../../exception.h"

using namespace std;
using namespace utf8;

// public static native Object get(Object array, int index)
//              throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static jobject _get(jobject array, jint index)
{
    if (array == nullptr) {
        throw java_lang_NullPointerException();
    }
    if (array->isArrayObject()) {
        throw java_lang_IllegalArgumentException("Argument is not an array");
    }

    auto arr = (Array *) array;
    if (index < 0 || index >= arr->arr_len) {
        throw java_lang_ArrayIndexOutOfBoundsException(to_string(index));
    }

    switch (arr->clazz->class_name[1]) {
    case 'Z': // boolean[]
        return boolBox(arr->get<jboolean>(index));
    case 'B': // byte[]
        return byteBox(arr->get<jbyte>(index));
    case 'C': // char[]
        return charBox(arr->get<jchar>(index));
    case 'S': // short[]
        return shortBox(arr->get<jshort>(index));
    case 'I': // int[]
        return intBox(arr->get<jint>(index));
    case 'J': // long[]
        return longBox(arr->get<jlong>(index));
    case 'F': // float[]
        return floatBox(arr->get<jfloat>(index));
    case 'D': // double[]
        return doubleBox(arr->get<jdouble>(index));
    default:  // reference array
        return arr->get<jref>(index);
    }
}

// public static native void set(Object array, int index, Object value)
//               throws IllegalArgumentException, ArrayIndexOutOfBoundsException;
static void set(jobject array, jint index, jobject value)
{
    if (array == nullptr) {
        throw java_lang_NullPointerException();
    }
    if (array->isArrayObject()) {
        throw java_lang_IllegalArgumentException("Argument is not an array");
    }

    auto arr = (Array *) array;
    if (index < 0 || index >= arr->arr_len) {
        throw java_lang_ArrayIndexOutOfBoundsException(to_string(index));
    }

    if (arr->isPrimArray() && value == nullptr) {
        // 基本类型的数组无法设空值
        throw java_lang_IllegalArgumentException();
    }

    switch (arr->clazz->class_name[1]) {
    case 'Z': // boolean[]
        if (!equals(value->clazz->class_name, S(java_lang_Boolean)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setBoolean(index, slot::getBool(value->unbox()));
        return;
    case 'B': // byte[]
        if (!equals(value->clazz->class_name, S(java_lang_Byte)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setByte(index, slot::getByte(value->unbox()));
        return;
    case 'C': // char[]
        if (!equals(value->clazz->class_name, S(java_lang_Character)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setChar(index, slot::getChar(value->unbox()));
        return;
    case 'S': // short[]
        if (!equals(value->clazz->class_name, S(java_lang_Short)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setShort(index, slot::getShort(value->unbox()));
        return;
    case 'I': // int[]
        if (!equals(value->clazz->class_name, S(java_lang_Integer)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setInt(index, slot::getInt(value->unbox()));
        return;    
    case 'J': // long[]
        if (!equals(value->clazz->class_name, S(java_lang_Long)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setLong(index, slot::getLong(value->unbox()));
        return;    
    case 'F': // float[]
        if (!equals(value->clazz->class_name, S(java_lang_Float)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setFloat(index, slot::getFloat(value->unbox()));
        return;    
    case 'D': // double[]
        if (!equals(value->clazz->class_name, S(java_lang_Double)))
            throw java_lang_IllegalArgumentException("argument type mismatch");
        else
            arr->setDouble(index, slot::getDouble(value->unbox()));
        return;    
    default:  // reference array
        arr->setRef(index, value);
        return;
    }
}

// public static native int getLength(Object array) throws IllegalArgumentException;
static jint getLength(jobject array)
{
    if (array == nullptr) {
        throw java_lang_NullPointerException();
    }
    if (!array->isArrayObject()) {
        throw java_lang_IllegalArgumentException("Argument is not an array");
    }

    auto arr = dynamic_cast<Array *>(array);
    assert(arr != nullptr);
    return arr->arr_len;
}

// private static native Object newArray(Class<?> componentType, int length)
//                  throws NegativeArraySizeException;
static jobject newArray_(jclass component_type, jint length)
{
    if (component_type == nullptr) {
        throw java_lang_NullPointerException();
    }
    if (length < 0) {
        throw java_lang_NegativeArraySizeException();
    }
    return component_type->jvm_mirror->arrayClass()->allocArray(length);
}

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "get", _OBJ "I)" OBJ, TA(_get) },
        { "set", _OBJ "ILjava/lang/Object;)V", TA(set) },
        { "getLength", _OBJ ")I", TA(getLength) },
        { "newArray", _CLS "I)" OBJ, TA(newArray_) },
};

void java_lang_reflect_Array_registerNatives()
{
    registerNatives("java/lang/reflect/Array", methods, ARRAY_LENGTH(methods));
}