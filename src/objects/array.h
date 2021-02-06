#ifndef CABIN_ARRAY_H
#define CABIN_ARRAY_H

#include <string>
#include "object.h"
#include "../metadata/class.h"

// Object of array
class Array: public Object {
    Array(Class *ac, jint arrLen);
    Array(Class *ac, jint dim, const jint lens[]);

public:
    jsize arr_len;     // present only if array Object

    [[nodiscard]] bool isArrayObject() const override { return true; }
    [[nodiscard]] bool isPrimArray() const;

    bool checkBounds(jint index)
    {
        return 0 <= index && index < arr_len;
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
        assert(0 <= i && i < arr_len);         \
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
    [[nodiscard]] size_t size() const override;
    [[nodiscard]] Array *clone() const override;
    [[nodiscard]] std::string toString() const override;

    friend class Class;
};

/*
 * [[[I -> int
 * [Ljava/lang/Object; -> java/lang/Object
 */
std::string arrClassName2eleClassName(const utf8_t *arr_class_name);

#define newArray(arr_class_name, arr_len) loadArrayClass(arr_class_name)->allocArray(arr_len)
#define newStringArray(arr_len) newArray(S(array_java_lang_String), arr_len)
#define newClassArray(arr_len)  newArray(S(array_java_lang_Class), arr_len)
#define newObjectArray(arr_len) newArray(S(array_java_lang_Object), arr_len)

#endif //CABIN_ARRAY_H
