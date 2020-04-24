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
    slots_mgr.init(data);
}

Object *Object::newObject(Class *c)
{
    size_t size = sizeof(Object) + c->instFieldsCount * sizeof(slot_t);
    return new(g_heap.allocObject(size)) Object(c);
}

Field *Object::lookupField(const char *name, const char *descriptor)
{
    assert(name != nullptr && descriptor != nullptr);
    return clazz->lookupInstField(name, descriptor);
}

Object *Object::clone() const
{
    size_t s = size();
    return (Object *) memcpy(g_heap.allocObject(s), this, s);
}

void Object::setByteField(Field *f, jbyte v)
{
    assert(f != nullptr);
    slots_mgr.setByte(f->id, v);
}

void Object::setBoolField(Field *f, jbool v)
{
    assert(f != nullptr);
    slots_mgr.setBool(f->id, v);
}

void Object::setCharField(Field *f, jchar v)
{
    assert(f != nullptr);
    slots_mgr.setChar(f->id, v);
}

void Object::setShortField(Field *f, jshort v)
{
    assert(f != nullptr);
    slots_mgr.setShort(f->id, v);
}

void Object::setIntField(Field *f, jint v)
{
    assert(f != nullptr);
    slots_mgr.setInt(f->id, v);
//    ISLOT(data + f->id) = v;
}

void Object::setFloatField(Field *f, jfloat v)
{
    assert(f != nullptr);
    slots_mgr.setFloat(f->id, v);
//    FSLOT(data + f->id) = v;
}

void Object::setLongField(Field *f, jlong v)
{
    assert(f != nullptr);
    slots_mgr.setLong(f->id, v);
//    LSLOT(data + f->id) = v;
}

void Object::setDoubleField(Field *f, jdouble v)
{
    assert(f != nullptr);
    slots_mgr.setDouble(f->id, v);
//    DSLOT(data + f->id) = v;
}

void Object::setRefField(Field *f, jref v)
{
    assert(f != nullptr);
    slots_mgr.setRef(f->id, v);
//    RSLOT(data + f->id) = v;
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

void Object::setFieldValue(int id, jref value)
{
    Field *f = clazz->getDeclaredInstField(id);

    if (value == jnull) {
//        RSLOT(data + id) = jnull;
        setRefField(f, jnull);
    } else if (f->isPrim()) {
        const slot_t *unbox = value->unbox();
        data[id] = *unbox;
        if (f->category_two)
            data[id+1] = *++unbox;
    } else {
        setRefField(f, jnull);
//        RSLOT(data + id) = jnull;
    }
}

jbyte Object::getByteField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getByte(f->id);
}

jbool Object::getBoolField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getBool(f->id);
}

jchar Object::getCharField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getChar(f->id);
}

jshort Object::getShortField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getShort(f->id);
}

jint Object::getIntField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getInt(f->id);
}

jfloat Object::getFloatField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getFloat(f->id);
}

jlong Object::getLongField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getLong(f->id);
}

jdouble Object::getDoubleField(Field *f)
{
    assert(f != nullptr);
    return slots_mgr.getDouble(f->id);
}

bool Object::isInstanceOf(Class *c) const
{
    if (c == nullptr)  // todo
        return false;
    return clazz->isSubclassOf(c);
}

const slot_t *Object::unbox() const
{
    return primObjUnbox((jprimref) this);
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
    return strObjToUtf8((jstrref) this);
}

string Object::toString() const
{
    ostringstream os;
    os << "Object(" << this << "), " << clazz->className;
    return os.str();
}
