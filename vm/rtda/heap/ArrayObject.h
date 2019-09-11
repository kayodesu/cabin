/*
 * Author: kayo
 */

#ifndef JVM_ARROBJ_H
#define JVM_ARROBJ_H

#include <cstddef>
#include <string>
#include "Object.h"
#include "../ma/Class.h"

// Object of array
class ArrayObject: public Object {
    size_t eleSize; // 表示数组每个元素的大小 todo 这个值在 ArrayClass 中有，保存在这里为了方便

    // 创建一维数组
    ArrayObject(ArrayClass *ac, jint arrLen);

    // 创建多维数组
    ArrayObject(ArrayClass *ac, size_t arr_dim, const size_t *arr_lens);

public:
    jsize len; // 数组的长度

    // 创建一维数组
    static ArrayObject *newInst(ArrayClass *ac, jint arrLen);

    // 创建多维数组
    static ArrayObject *newInst(ArrayClass *ac, size_t arr_dim, const size_t *arr_lens);

    static void operator delete(void *rawMemory,std::size_t size) throw();

    bool checkBounds(jint index)
    {
        return 0 <= index && index < len;
    }

    void *index(jint index0) const
    {
        assert(0 <= index0 && index0 < len);
        return ((u1 *) (data)) + eleSize*index0;
    }

    template <typename T>
    void set(jint index0, T data)
    {
        *(T *) index(index0) = data;
    }

    template <typename T>
    T get(jint index0) const
    {
        return *(T *) index(index0);
    }

    static void copy(ArrayObject *dst, jint dst_pos, const ArrayObject *src, jint src_pos, jint len);
    size_t size() const override;
    std::string toString() const override;
};

#endif //JVM_ARROBJ_H
