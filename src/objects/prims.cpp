#include <algorithm>
#include "prims.h"
#include "../symbol.h"
#include "../util/encoding.h"
#include "../kayo.h"
#include "field.h"
#include "object.h"
#include "class.h"

/*
 * Author: kayo
 */

using namespace std;
using namespace utf8;

/*
 * 基本类型的名称，描述符，等等
 */
static const struct {
    const utf8_t *class_name;
    utf8_t descriptor;
    const utf8_t *array_class_name;
    const utf8_t *wrapper_class_name;
} prims[] = {
        { S(void),   'V', S(array_V), S(java_lang_Void) },
        { S(boolean), 'Z', S(array_Z), S(java_lang_Boolean) },
        { S(byte),    'B', S(array_B), S(java_lang_Byte) },
        { S(char),   'C', S(array_C), S(java_lang_Character) },
        { S(short),  'S', S(array_S), S(java_lang_Short) },
        { S(int),    'I', S(array_I), S(java_lang_Integer) },
        { S(long),   'J', S(array_J), S(java_lang_Long) },
        { S(float),  'F', S(array_F), S(java_lang_Float) },
        { S(double), 'D', S(array_D), S(java_lang_Double) }
};

bool isPrimClassName(const utf8_t *class_name)
{
    assert(class_name != nullptr);
    return find_if(begin(prims), end(prims),
                   [=](auto &prim){ return equals(prim.class_name, class_name); }) != end(prims);
}

bool isPrimDescriptor(utf8_t descriptor)
{
    return find_if(begin(prims), end(prims),
                   [=](auto &prim){ return prim.descriptor == descriptor; }) != end(prims);
}

const utf8_t *getPrimArrayClassName(const utf8_t *class_name)
{
    assert(class_name != nullptr);
    for (auto &t : prims) {
        if (equals(t.class_name, class_name))
            return t.array_class_name;
    }
    return nullptr;
}

const utf8_t *getPrimClassName(utf8_t descriptor)
{
    for (auto &t : prims) {
        if (t.descriptor == descriptor)
            return t.class_name;
    }
    return nullptr;
}

const slot_t *primObjUnbox(jprimref po)
{
    assert(po != nullptr);

    Class *c = po->clazz;
    if (!c->isPrimClass()) {
        jvm_abort("error"); // todo
    }

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    Field *f = c->lookupField(S(value), c->className);
    if (f == nullptr) {
        jvm_abort("error, %s, %s\n", S(value), c->className); // todo
    }
    return po->data + f->id;
}
