#include "array.h"
#include "prims.h"
#include "../runtime/vm_thread.h"
#include "../exception.h"

using namespace std;

Array::Array(Class *ac, jint arr_len): Object(ac)
{
    assert(ac != nullptr);
    assert(ac->isArrayClass());
    assert(arr_len >= 0); // 长度为0的array是合法的

    this->arr_len = arr_len;
    // java 数组创建后要赋默认值，0, 0.0, false,'\0', NULL 之类的
    // heap 申请对象时已经清零了。
    data = (slot_t *) (this + 1);
}

Array::Array(Class *ac, jint dim, const jint lens[]): Object(ac)
{
    assert(ac != nullptr);
    assert(dim >= 1);
    assert(ac->isArrayClass());

    arr_len = lens[0];
    assert(arr_len >= 0); // 长度为0的array是合法的

    data = (slot_t *) (this + 1);

    for (int d = 1; d < dim; d++) {
        for (int i = 0; i < arr_len; i++) {
            setRef(i, ac->componentClass()->allocMultiArray(dim - 1, lens + 1));
        }
    }
}

bool Array::isPrimArray() const
{
    return isPrimDescriptor(clazz->class_name[1]);
}

void *Array::index(jint index0) const
{
    assert(0 <= index0 && index0 < arr_len);
    return ((u1 *) (data)) + clazz->getEleSize()*index0;
}

void Array::setRef(int i, jref value)
{
    assert(0 <= i && i < arr_len);
    assert(clazz->isRefArrayClass());

    auto data = (slot_t *) index(i);
    if (value == jnull) {
        *data = (slot_t) jnull;
    } else if (isPrimArray()) {
        const slot_t *unbox = value->unbox();
        *data = *unbox;
        if (clazz->ele_size > sizeof(slot_t))
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
        throw java_lang_ArrayStoreException();
    }

    if (src_pos < 0
        || dst_pos < 0
        || len < 0
        || src_pos + len > src->arr_len
        || dst_pos + len > dst->arr_len) {
        throw java_lang_ArrayIndexOutOfBoundsException();
    }

    memcpy(dst->index(dst_pos), src->index(src_pos), src->clazz->getEleSize() * len);
}

size_t Array::size() const
{
    return clazz->objectSize(arr_len);
//    return sizeof(Array) + clazz->getEleSize()*len;
}

Array *Array::clone() const
{
    size_t s = size();
    void *p = g_heap->alloc(s);
    memcpy(p, this, s);

    // todo mutex 怎么处理

    auto clone = (Array *) p;
    clone->data = (slot_t *) (clone + 1);
    return clone;
}

string Array::toString() const
{
    // todo
    string s;
    return s;
}

string arrClassName2eleClassName(const utf8_t *arr_class_name)
{
    assert(arr_class_name != nullptr);
    assert(arr_class_name[0] == '['); // must be array class name

    auto ele_name = arr_class_name;
    while (*++ele_name == '[');

    auto prim_class_name = getPrimClassName(*ele_name);
    if (prim_class_name != nullptr) {  // primitive type
        return prim_class_name;
    }

    // 普通类: Lxx/xx/xx; 型
    assert(*ele_name == 'L');
    ele_name++; // jump 'L'

    size_t last = strlen(ele_name) - 1;
    assert(last > 0);
    assert(ele_name[last] == ';');

    char buf[last + 1];
    strncpy(buf, ele_name, (size_t) last);
    buf[last] = 0;
    return buf;
}