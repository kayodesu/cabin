#include <sstream>
#include "../symbol.h"
#include "class_loader.h"
#include "object.h"
#include "array.h"
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

Field *Object::lookupField(const char *name, const char *descriptor)
{
    assert(name != nullptr && descriptor != nullptr);
    return clazz->lookupInstField(name, descriptor);
}

Object *Object::clone() const
{
    if (clazz == g_class_class) {
        JVM_PANIC("Object of java.lang.Class don't support clone"); // todo
    }

    size_t s = size();
    void *p = g_heap->alloc(s);
    memcpy(p, this, s);

    // todo mutex 怎么处理

    Object *clone = (Object *) p;
    clone->data = (slot_t *) (clone + 1);
    return clone;
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
    return clazz->objectSize();
//    return sizeof(*this) + clazz->inst_fields_count * sizeof(slot_t);
}

bool Object::isArrayObject() const
{
    return clazz->class_name[0] == '[';
}

bool Object::isClassObject() const
{
    return clazz == g_class_class;
}

string Object::toString() const
{
    ostringstream os;
    os << "Object(" << this << "), " << clazz->class_name;
    return os.str();
}

static utf8_t *string2utf8(jstrref so)
{
    assert(so != nullptr);
    assert(g_string_class != nullptr);
    assert(so->clazz == g_string_class);

    if (g_jdk_version_9_and_upper) {
        // byte[] value;
        auto value = so->getRefField<Array>(S(value), S(array_B));
        static_assert(sizeof(utf8_t) == sizeof(jbyte), ""); // todo
        auto utf8 = new utf8_t[value->arr_len + 1];
        utf8[value->arr_len] = 0;
        memcpy(utf8, value->data, value->arr_len * sizeof(jbyte));
        return utf8;
    } else {
        // char[] value;
        auto value = so->getRefField<Array>(S(value), S(array_C));
        return unicode::toUtf8((const unicode_t *) (value->data), value->arr_len);
    }
}

utf8_t *Object::toUtf8() const
{
    return string2utf8((jstrref) this);
}

static Object *newString_jdk_8_and_under(const utf8_t *str)
{
    assert(g_string_class != nullptr && str != nullptr);

    initClass(g_string_class);
    auto strobj = g_string_class->allocObject();
    auto len = length(str);

    // set java/lang/String 的 value 变量赋值
    Array *value = newArray(S(array_C), len); // [C
    toUnicode(str, (unicode_t *) (value->data));
    strobj->setRefField(S(value), S(array_C), value);

    return strobj;
}

static Object *newString_jdk_9_and_upper(const utf8_t *str)
{
    assert(g_string_class != nullptr && str != nullptr);

    initClass(g_string_class);
    assert(is_jtrue(g_string_class->lookupField("COMPACT_STRINGS", "Z")->static_value.z));

    auto strobj = g_string_class->allocObject();
    auto len = length(str);

    // set java/lang/String 的 value 变量赋值
    // private final byte[] value;
    Array *value = newArray(S(array_B), len); // [B
    memcpy(value->data, str, len);
    strobj->setRefField(S(value), S(array_B), value);

    // set java/lang/String 的 coder 变量赋值
    // private final byte coder;
    // 可取一下两值之一：
    // @Native static final byte LATIN1 = 0;
    // @Native static final byte UTF16  = 1;
    strobj->setByteField(S(coder), "B", 0);
    return strobj;
}

jstrref newString(const utf8_t *str)
{
    if (g_jdk_version_9_and_upper) {
        return newString_jdk_9_and_upper(str);
    } else {
        return newString_jdk_8_and_under(str);
    }
}

jstrref newString(const unicode_t *str, jsize len)
{
    // todo
    JVM_PANIC("not implement.");
}

bool StringEquals::operator()(jstrref x, jstrref y) const
{
    assert(x != nullptr);
    assert(y != nullptr);
    assert(x->clazz == g_string_class);
    assert(y->clazz == g_string_class);

    // public boolean equals(Object anObject);
    Method *equals = g_string_class->getDeclaredInstMethod("equals", "(Ljava/lang/Object;)Z");
    return getBool(execJavaFunc(equals, { x, y })) != jfalse;
}

size_t StringHash::operator()(jstrref x) const
{
    assert(x != nullptr);
    assert(x->clazz == g_string_class);

    // public int hashCode();
    Method *hashCode = g_string_class->getDeclaredInstMethod("hashCode", "()I");
    return (size_t) getInt(execJavaFunc(hashCode, {x}));
}

jsize stringGetLength(jstrref so)
{
    // todo
    JVM_PANIC("not implement.");
}

jsize stringGetUTFLength(jstrref so)
{
    // todo
    JVM_PANIC("not implement.");
}
