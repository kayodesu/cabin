/*
 * Author: kayo
 */

#include <cstring>
#include <cassert>
#include "Array.h"
#include "../runtime/Thread.h"
#include "Prims.h"

using namespace std;

Array *Array::newArray(Class *ac, jint arrLen)
{
    assert(ac != nullptr);
    assert(ac->isArrayClass());
    size_t size = sizeof(Array) + ac->getEleSize()*arrLen;
    return new(g_heap.allocObject(size)) Array(ac, arrLen);
}

Array *Array::newMultiArray(Class *ac, jint dim, const jint lens[])
{
    assert(ac != nullptr);
    assert(dim >= 1);
    assert(ac->isArrayClass());

    size_t size = sizeof(Array) + ac->getEleSize()*lens[0];
    return new(g_heap.allocObject(size)) Array(ac, dim, lens);
}

Array::Array(Class *ac, jint arrLen): Object(ac), len(arrLen)
{
    assert(ac != nullptr);
    assert(ac->isArrayClass());
    assert(arrLen >= 0); // 长度为0的array是合法的

    // java 数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的
    // heap 申请对象时已经清零了。
    data = (slot_t *) (this + 1);
}

Array::Array(Class *ac, jint dim, const jint lens[]): Object(ac), len(lens[0])
{
    assert(ac != nullptr);
    assert(dim >= 1);
    assert(ac->isArrayClass());
    assert(len >= 0); // 长度为0的array是合法的

    data = (slot_t *) (this + 1);

    for (int d = 1; d < dim; d++) {
        for (int i = 0; i < len; i++) {
            set(i, newMultiArray(ac->componentClass(), dim - 1, lens + 1));
        }
    }
}

bool Array::isPrimArray() const
{
    return Prims::isPrimDescriptor(clazz->className[1]);
}

void Array::set(int index0, jref value)
{
    assert(0 <= index0 && index0 < len);

    auto data = (slot_t *) index(index0);
    if (value == jnull) {
        *data = (slot_t) jnull;
    } else if (isPrimArray()) {
        const slot_t *unbox = value->unbox();
        *data = *unbox;
        if (clazz->eleSize > sizeof(slot_t))
            *++data = *++unbox;
    } else {
        *data = (slot_t) value;
    }
}

void Array::copy(Array *dst, jint dst_pos, const Array *src, jint src_pos, jint len)
{
    assert(src != nullptr);
    assert(src->isArrayObject());

    assert(dst != nullptr);
    assert(dst->isArrayObject());

    if (len < 1) {
        // need to do nothing
        return;
    }

    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (src->clazz->getEleSize() != dst->clazz->getEleSize()) {
        thread_throw(new ArrayStoreException);
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->len
        || dst_pos + len > dst->len) {
        thread_throw(new IndexOutOfBoundsException);
    }

    memcpy(dst->index(dst_pos), src->index(src_pos), src->clazz->getEleSize() * len);
}

size_t Array::size() const
{
    return sizeof(Array) + clazz->getEleSize()*len;
}

//Array *Array::clone() const
//{
    //size_t s = size();
  //  return (Array *) memcpy(g_heap.allocObject(s), this, s);
//}

string Array::toString() const
{
    // todo
    string s;
    return s;
}

