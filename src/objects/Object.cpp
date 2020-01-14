/*
 * Author: kayo
 */

#include <sstream>
#include "../symbol.h"
#include "class_loader.h"
#include "Object.h"
#include "Class.h"
#include "Field.h"
#include "Array.h"
#include "../interpreter/interpreter.h"
#include "Prims.h"

using namespace std;
using namespace utf8;

Object::Object(Class *c): clazz(c)
{
    data = (slot_t *) (this + 1);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); // 同一线程可重入的锁

    pthread_mutex_init(&mutex, &attr);
}

void Object::lock()
{
    pthread_mutex_lock(&mutex);
}

void Object::unlock()
{
    pthread_mutex_unlock(&mutex);
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
    strobj->setFieldValue(S(value), S(array_C), (slot_t) value);

    return strobj;
}

Object *Object::clone() const
{
    size_t s = size();
    return (Object *) memcpy(g_heap.allocObject(s), this, s);
}

void Object::setFieldValue(Field *f, slot_t v)
{
    assert(f != nullptr);
    assert(!f->isStatic());

    if (!f->categoryTwo) {
        data[f->id] = v;
    } else { // categoryTwo
        data[f->id] = 0; // 高字节清零
        data[f->id + 1] = v; // 低字节存值
    }
}

void Object::setFieldValue(Field *f, const slot_t *value)
{
    assert(f != nullptr && !f->isStatic() && value != nullptr);

    data[f->id] = value[0];
    if (f->categoryTwo) {
        data[f->id + 1] = value[1];
    }
}

void Object::setFieldValue(const char *name, const char *descriptor, slot_t v)
{
    assert(name != nullptr && descriptor != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), v);
}

void Object::setFieldValue(const char *name, const char *descriptor, const slot_t *value)
{
    assert(name != nullptr && descriptor != nullptr && value != nullptr);
    setFieldValue(clazz->lookupInstField(name, descriptor), value);
}

void Object::setFieldValue(int id, jref value)
{
    Field *f = clazz->getDeclaredInstField(id);

    if (value == jnull) {
        data[id] = (slot_t) jnull;
    } else if (f->isPrim()) {
        const slot_t *unbox = value->unbox();
        data[id] = *unbox;
        if (f->categoryTwo)
            data[id+1] = *++unbox;
    } else {
        data[id] = (slot_t) value;
    }
}

const slot_t *Object::getInstFieldValue0(const char *name, const char *descriptor) const
{
    assert(name != nullptr && descriptor != nullptr);

    Field *f = clazz->lookupField(name, descriptor);
    assert(f != nullptr);
    return data + f->id;
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
