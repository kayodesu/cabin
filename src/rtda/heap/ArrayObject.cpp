/*
 * Author: kayo
 */

#include <cstring>
#include <cassert>
#include "ArrayObject.h"
#include "../ma/ArrayClass.h"

using namespace std;

ArrayObject *ArrayObject::newInst(ArrayClass *ac, jint arrLen)
{
    size_t size = sizeof(ArrayObject) + ac->getEleSize()*arrLen;
    return new(g_heap_mgr.get(size)) ArrayObject(ac, arrLen);
}

ArrayObject *ArrayObject::newInst(ArrayClass *arrClass, size_t arrDim, const size_t *arrLens)
{
    int count = 1;
    for (size_t i = 0; i < arrDim; i++) {
        count *= arrLens[i];
    }

    size_t size = sizeof(ArrayObject) + arrClass->getEleSize()*count;
    return new(g_heap_mgr.get(size)) ArrayObject(arrClass, arrDim, arrLens);
}

void ArrayObject::operator delete(void *rawMemory,std::size_t size) throw()
{
    Object::operator delete(rawMemory, size);
}

ArrayObject::ArrayObject(ArrayClass *arrClass, jint arrLen): Object(arrClass), len(arrLen)
{
    assert(arrClass != nullptr);
    assert(arrClass->isArray());
    assert(arrLen >= 0); // todo 等于0合不合法?

    eleSize = arrClass->getEleSize();
    data = reinterpret_cast<slot_t *>(this + 1); // todo data怎么办,这样不对
    // java数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的 todo
    memset(data, 0, eleSize * arrLen);
}

ArrayObject::ArrayObject(ArrayClass *arrClass, size_t arr_dim, const size_t *arr_lens): Object(arrClass)
{
    jvm_abort("此方法实现完全错误！"); // todo

    assert(arrClass != nullptr);
    assert(arr_lens != nullptr);
    assert(arrClass->isArray());
    assert(arr_dim >= 0); // todo 等于0合不合法?

    eleSize = arrClass->getEleSize();
    data = reinterpret_cast<slot_t *>(this + 1);  // todo data怎么办,这样不对

    /*
     * 多维数组是数组的数组
     * 先创建其第一维，第一维的每个元素也是一数组
     */
    size_t len = arr_lens[0];
    new(this) ArrayObject(arrClass, len); // todo 这样感觉不对
    if (arr_dim == 1) {
        return;
    }

    // todo
    auto eleClass = (ArrayClass *)(arrClass->loader->loadClass(arrClass->className + 1)); // jump '['
    for (size_t i = 0; i < len; i++) {
        set(i, new ArrayObject(eleClass, arr_dim - 1, arr_lens + 1));
    }
}

void ArrayObject::copy(ArrayObject *dst, jint dst_pos, const ArrayObject *src, jint src_pos, jint len)
{
    assert(src != nullptr);
    assert(src->isArray());

    assert(dst != nullptr);
    assert(dst->isArray());

    if (len < 1) {
        // need to do nothing
        return;
    }

    /*
     * 首先确保src和dst都是数组，然后检查数组类型。
     * 如果两者都是引用数组，则可以拷贝，否则两者必须是相同类型的基本类型数组
     */
    if (src->eleSize != dst->eleSize) {
        // todo error  ArrayStoreException
        printvm("ArrayStoreException\n");
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->len
        || dst_pos + len > dst->len) {
        // todo "java.lang.IndexOutOfBoundsException"
        jvm_abort("java.lang.IndexOutOfBoundsException\n");
    }

    memcpy(dst->index(dst_pos), src->index(src_pos), src->eleSize * len);
}

size_t ArrayObject::size() const
{
    return sizeof(*this) + eleSize*len;;
}

string ArrayObject::toString() const
{
    // todo
    string s;
    return s;
}

