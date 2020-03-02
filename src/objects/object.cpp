/*
 * Author: kayo
 */

#include <sstream>
#include "../symbol.h"
#include "class_loader.h"
#include "object.h"
#include "class.h"
#include "../interpreter/interpreter.h"
#include "Prims.h"
#include "../runtime/thread_info.h"

using namespace std;
using namespace utf8;

Object::Object(Class *c): clazz(c)
{
    data = (slot_t *) (this + 1);
}

Object *Object::newObject(Class *c)
{
    size_t size = sizeof(Object) + c->instFieldsCount * sizeof(slot_t);
    return new(g_heap.allocObject(size)) Object(c);
}

Object *Object::newString(const utf8_t *str)
{
    assert(stringClass != nullptr);

    initClass(stringClass);
    auto strobj =  newObject(stringClass);
    auto len = length(str);

    // set java/lang/String 的 value 变量赋值
    Array *value = newArray(loadBootClass(S(array_C) /* [C */ ), len);
    toUnicode(str, (unicode_t *) (value->data));
    strobj->setFieldValue(S(value), S(array_C), value);

    return strobj;
}

Object *Object::clone() const
{
    size_t s = size();
    return (Object *) memcpy(g_heap.allocObject(s), this, s);
}

void Object::setFieldValue(Field *f, jint v)
{
    assert(f != nullptr);
    ISLOT(data + f->id) = v;
}

void Object::setFieldValue(Field *f, jfloat v)
{
    assert(f != nullptr);
    FSLOT(data + f->id) = v;
}

void Object::setFieldValue(Field *f, jlong v)
{
    assert(f != nullptr);
    LSLOT(data + f->id) = v;
}

void Object::setFieldValue(Field *f, jdouble v)
{
    assert(f != nullptr);
    DSLOT(data + f->id) = v;
}

void Object::setFieldValue(Field *f, jref v)
{
    assert(f != nullptr);
    RSLOT(data + f->id) = v;
}

//void Object::setFieldValue(Field *f, slot_t v)
//{
//    assert(f != nullptr);
//    assert(!f->isStatic());
//
//    if (!f->category_two) {
//        data[f->id] = v;
//    } else { // categoryTwo
//        data[f->id] = 0; // 高字节清零
//        data[f->id + 1] = v; // 低字节存值
//    }
//}

void Object::setFieldValue(Field *f, const slot_t *value)
{
    assert(f != nullptr && !f->isStatic() && value != nullptr);

    data[f->id] = value[0];
    if (f->category_two) {
        data[f->id + 1] = value[1];
    }
}

void Object::setFieldValue(const char *name, const char *descriptor, jint v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

void Object::setFieldValue(const char *name, const char *descriptor, jfloat v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

void Object::setFieldValue(const char *name, const char *descriptor, jlong v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

void Object::setFieldValue(const char *name, const char *descriptor, jdouble v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

void Object::setFieldValue(const char *name, const char *descriptor, jref v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

//void Object::setFieldValue(const char *name, const char *descriptor, slot_t v)
//{
//    assert(name != nullptr && descriptor != nullptr);
//    setFieldValue(clazz->lookupInstField(name, descriptor), v);
//}

//void Object::setFieldValue(const char *name, const char *descriptor, const slot_t *value)
//{
//    assert(name != nullptr && descriptor != nullptr && value != nullptr);
//    setFieldValue(clazz->lookupInstField(name, descriptor), value);
//}

void Object::setFieldValue(int id, jref value)
{
    Field *f = clazz->getDeclaredInstField(id);

    if (value == jnull) {
        RSLOT(data + id) = jnull;
    } else if (f->isPrim()) {
        const slot_t *unbox = value->unbox();
        data[id] = *unbox;
        if (f->category_two)
            data[id+1] = *++unbox;
    } else {
        RSLOT(data + id) = jnull;
    }
}

const slot_t *Object::getInstFieldValue0(const Field *f) const
{
    assert(f != nullptr);
    return data + f->id;
}

const slot_t *Object::getInstFieldValue0(const char *name, const char *descriptor) const
{
    assert(name != nullptr && descriptor != nullptr);
    Field *f = clazz->lookupField(name, descriptor);
    assert(f != nullptr);
    return getInstFieldValue0(f);
}

bool Object::isInstanceOf(Class *c) const
{
    if (c == nullptr)  // todo
        return false;
    return clazz->isSubclassOf(c);
}

const slot_t *Object::unbox() const
{
    if (!clazz->isPrimClass()) {
        jvm_abort(""); // todo
    }

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    Field *f = clazz->lookupField(S(value), clazz->className);
    if (f == nullptr) {
        jvm_abort("error, %s, %s\n", S(value), clazz->className); // todo
    }
    return data + f->id;
}

size_t Object::size() const
{
    assert(clazz != nullptr);
    return sizeof(*this) + clazz->instFieldsCount * sizeof(slot_t);
}

bool Object::isArrayObject() const
{
    return clazz->className[0] == '[';
}

utf8_t *Object::toUtf8() const
{
    assert(clazz != nullptr);
    assert(stringClass != nullptr);
    assert(clazz == stringClass);

    auto value = getInstFieldValue<Array *>(S(value), S(array_C));
    return unicode::toUtf8((const unicode_t *) (value->data), value->len);
}

string Object::toString() const
{
    ostringstream os;
    os << "Object(" << this << "), " << clazz->className;
    return os.str();
}

bool StrObjEquals::operator()(Object *x, Object *y) const
{
    assert(x != nullptr);
    assert(y != nullptr);
    assert(x->clazz == stringClass);
    assert(y->clazz == stringClass);

    // public boolean equals(Object anObject);
    Method *equals = stringClass->getDeclaredInstMethod("equals", "(Ljava/lang/Object;)Z");
    return *(jint *)execJavaFunc(equals, x, y) != 0;
}

size_t StrObjHash::operator()(Object *x) const
{
    assert(x != nullptr);
    assert(x->clazz == stringClass);

    // public int hashCode();
    Method *hashCode = stringClass->getDeclaredInstMethod("hashCode", "()I");
    return (size_t) *(jint *)execJavaFunc(hashCode, x);

}

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

void *Array::index(jint index0) const
{
    assert(0 <= index0 && index0 < len);
    return ((u1 *) (data)) + clazz->getEleSize()*index0;
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
