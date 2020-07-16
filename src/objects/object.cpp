/*
 * Author: Yo Ka
 */

#include <sstream>
#include "../symbol.h"
#include "class_loader.h"
#include "object.h"
#include "array_object.h"
#include "../metadata/class.h"
#include "../metadata/field.h"
#include "../interpreter/interpreter.h"
#include "prims.h"
#include "../runtime/vm_thread.h"

using namespace std;
using namespace utf8;
using namespace slot;

Object::Object(Class *c): clazz(c)
{
    data = (slot_t *) (this + 1);
}

Object *Object::newObject(Class *c)
{
    size_t size = sizeof(Object) + c->inst_field_count * sizeof(slot_t);
    return new (g_heap->alloc(size)) Object(c);
}

Field *Object::lookupField(const char *name, const char *descriptor)
{
    assert(name != nullptr && descriptor != nullptr);
    return clazz->lookupInstField(name, descriptor);
}

Object *Object::clone() const
{
    size_t s = size();
    return (Object *) memcpy(g_heap->alloc(s), this, s);
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

bool Object::isInstanceOf(Class *c) const
{
    if (c == nullptr)  // todo
        return false;
    return clazz->isSubclassOf(c);
}

const slot_t *Object::unbox() const
{
    assert(clazz->isPrimWrapperClass());
    return primObjUnbox(this);
}

size_t Object::size() const
{
    assert(clazz != nullptr);
    return sizeof(*this) + clazz->inst_field_count * sizeof(slot_t);
}

bool Object::isArrayObject() const
{
    return clazz->class_name[0] == '[';
}

utf8_t *Object::toUtf8() const
{
    return strObjToUtf8((jstrref) this);
}

string Object::toString() const
{
    ostringstream os;
    os << "Object(" << this << "), " << clazz->class_name;
    return os.str();
}
