/*
 * Author: kayo
 */

#ifndef KAYO_ARRAY_OBJECT_H
#define KAYO_ARRAY_OBJECT_H

#include "object.h"

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

    template <typename T>
    void set(jint index0, T data)
    {
        *(T *) index(index0) = data;
    }

    void set(int index0, jref value);

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

#endif //KAYO_ARRAY_OBJECT_H
