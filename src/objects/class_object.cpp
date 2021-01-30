#include <cassert>
#include "../metadata/class.h"
#include "class_object.h"

ClassObject::ClassObject(Class *c): Object(g_class_class), jvm_mirror(c)
{
    assert(g_class_class != nullptr);
    assert(jvm_mirror != nullptr);

    data = (slot_t *) (this + 1);
    // private final ClassLoader classLoader;
    setRefField("classLoader", S(sig_java_lang_ClassLoader), c->loader);
}

Object *ClassObject::clone() const
{
    jvm_abort("ClassObject don't support clone"); // todo
}
