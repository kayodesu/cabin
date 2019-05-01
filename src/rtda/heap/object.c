/*
 * Author: Jia Yang
 */

#include <string.h>
#include <stdlib.h>
#include "object.h"
#include "../ma/field.h"
#include "mgr/heap_mgr.h"
#include "../../symbol.h"

Object *object_create(Class *c)
{
    assert(c != NULL);
    assert(!class_is_array(c));

    size_t size = sizeof(Object) + c->instance_fields_count * sizeof(slot_t);
    Object *o = hm_get(&g_heap_mgr, size);
    o->clazz = c;
    o->size = size;
    return o;
}

Object *object_clone(const Object *src)
{
    assert(src != NULL);
    Object *o = hm_get(&g_heap_mgr, src->size);
    memcpy(o, src, src->size);
    return o;
}

void set_instance_field_value(Object *o, Field *f, const slot_t *value)
{
    assert(o != NULL && f != NULL && value != NULL);

    o->data[f->id] =  value[0];
    if (f->category_two) {
        o->data[f->id + 1] = value[1];
    }
}

const slot_t *get_instance_field_value_by_id(const Object *o, int id)
{
    assert(o != NULL);
    assert(0 <= id && id < o->clazz->instance_fields_count);
    return o->data + id;
}

const slot_t *get_instance_field_value(const Object *o, Field *f)
{
    assert(o != NULL);
    assert(f != NULL);
    return o->data + f->id;
}

const slot_t *get_instance_field_value_by_nt(const Object *o, const char *name, const char *descriptor)
{
    assert(o != NULL && name != NULL && descriptor != NULL);

    Field *f = class_lookup_field(o->clazz, name, descriptor);
    if (f == NULL) {
        jvm_abort("error, %s, %s\n", name, descriptor); // todo
    }

    return get_instance_field_value(o, f);
}

bool object_is_instance_of(const Object *o, const Class *c)
{
    if (o == NULL || c == NULL)  // todo
        return false;
    return class_is_subclass_of(o->clazz, c);
}

const slot_t *priobj_unbox(const Object *po)
{
    assert(po != NULL);
    assert(is_primitive(po->clazz));

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    return get_instance_field_value_by_nt(po, S(value), po->clazz->class_name);
}

void object_destroy(Object *o)
{
    if (o == NULL) {
        // todo
    }

    // todo
}

const char *object_to_string(const Object *o)
{
#define MAX_LEN 1023 // big enough? todo
    char *result = vm_malloc(sizeof(char)*(MAX_LEN + 1));
    if (o == NULL) {
        strcpy(result, "object is null");
        return result;
    }

    int n = snprintf(result, MAX_LEN, "object(%p), %s", o, o->clazz->class_name);
    // todo extra
    assert(0 <= n && n <= MAX_LEN);
    result[n] = 0;
    return result;
}
