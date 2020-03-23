/*
 * Author: kayo
 */

#include <sstream>
#include "../symbol.h"
#include "class_loader.h"
#include "object.h"
#include "array_object.h"
#include "class.h"
#include "field.h"
#include "../interpreter/interpreter.h"
#include "prims.h"
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

//void Object::setFieldValue(const char *name, const char *type, slot_t v)
//{
//    assert(name != nullptr && type != nullptr);
//    setFieldValue(clazz->lookupInstField(name, type), v);
//}

//void Object::setFieldValue(const char *name, const char *type, const slot_t *value)
//{
//    assert(name != nullptr && type != nullptr && value != nullptr);
//    setFieldValue(clazz->lookupInstField(name, type), value);
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

    if (g_jdk_version_9_and_upper) {
        auto value = getInstFieldValue<Array *>(S(value), S(array_B));
        assert(sizeof(utf8_t) == sizeof(jbyte));
        auto utf8 = new utf8_t[value->len + 1];
        utf8[value->len] = 0;
        memcpy(utf8, value->data, value->len * sizeof(jbyte));
        return utf8;
    } else {
        auto value = getInstFieldValue<Array *>(S(value), S(array_C));
        return unicode::toUtf8((const unicode_t *) (value->data), value->len);
    }
}

string Object::toString() const
{
    ostringstream os;
    os << "Object(" << this << "), " << clazz->className;
    return os.str();
}
