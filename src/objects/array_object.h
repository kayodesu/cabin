/*
 * Author: Yo Ka
 */

#ifndef KAYO_ARRAY_OBJECT_H
#define KAYO_ARRAY_OBJECT_H

#include <string>
#include "object.h"
#include "../metadata/class.h"

// Object of array
class Array: public Object {
    Array(Class *ac, jint arrLen);
    Array(Class *ac, jint dim, const jint lens[]);

public:
    jsize len; // 数组的长度

    static Array *newArray(Class *ac, jint arrLen);
    static Array *newMultiArray(Class *ac, jint dim, const jint lens[]);

    bool isArrayObject() const override { return true; }
    bool isPrimArray() const;

    bool checkBounds(jint index)
    {
        return 0 <= index && index < len;
    }

    void *index(jint index0) const;

    // set prim type value
    // template <typename T>
    // void set(jint index0, T data)
    // {
    //     if(!((typeid(T) == typeid(jbyte)) 
    //             || (typeid(T) == typeid(jbool)) 
    //             || (typeid(T) == typeid(jchar)) 
    //             || (typeid(T) == typeid(jshort)) 
    //             || (typeid(T) == typeid(jint)) 
    //             || (typeid(T) == typeid(jlong)) 
    //             || (typeid(T) == typeid(jfloat))
    //             || (typeid(T) == typeid(jdouble)))) {
    //         printf("--- %s\n", typeid(T).name());
    //             }
    //     *(T *) index(index0) = data;
    // }

#define setT(jtype, Type)                      \
    void set##Type(jint i, jtype v)            \
    {                                          \
        assert(clazz->is##Type##ArrayClass()); \
        assert(0 <= i && i < len);             \
        *(jtype *) index(i) = v;               \
    }  

    setT(jbyte, Byte)
    setT(jboolean, Boolean)
    setT(jchar, Char)
    setT(jshort, Short)
    setT(jint, Int)
    setT(jlong, Long)
    setT(jfloat, Float)
    setT(jdouble, Double)
#undef setT

    void setRef(int i, jref value);

    template <typename T>
    T get(jint index0) const
    {
        return *(T *) index(index0);
    }

    static void copy(Array *dst, jint dst_pos, const Array *src, jint src_pos, jint len);
    size_t size() const override;
    //Array *clone() const;
    std::string toString() const override;
};


static inline Array *newArray(Class *ac, jint arrLen)
{
    return Array::newArray(ac, arrLen);
}

Array *newTypeArray(ArrayType type, jint arr_len);

static inline Array *newMultiArray(Class *ac, jint dim, const jint lens[])
{
    return Array::newMultiArray(ac, dim, lens);
}

/*
 * [[[I -> int
 * [Ljava/lang/Object; -> java/lang/Object
 */
std::string arrClassName2EleClassName(const utf8_t *arr_class_name);

#endif //KAYO_ARRAY_OBJECT_H
