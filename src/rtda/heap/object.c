/*
 * Author: Jia Yang
 */

#include <string.h>
#include <stdlib.h>
#include "object.h"
#include "../ma/field.h"
#include "mgr/heap_mgr.h"

struct object* object_create(struct class *c)
{
    assert(c != NULL);
    assert(!class_is_array(c));

    size_t size = sizeof(struct object) + c->instance_fields_count * sizeof(slot_t);
    struct object *o = hm_get(&g_heap_mgr, size);
    o->clazz = c;
    o->size = size;
//    memcpy(o->data, c->inited_instance_fields_values, c->instance_fields_count * sizeof(struct slot));
    return o;
}

//void* jarrobj_copy_data(const struct object *o);

struct object* object_clone(const struct object *src)
{
    assert(src != NULL);
    struct object *o = hm_get(&g_heap_mgr, src->size);
    memcpy(o, src, src->size);
    return o;

//    o->clazz = src->clazz;
//    o->instance_fields_count = src->instance_fields_count;
//
//    if (jobject_is_array(src)) {
//        o->instance_fields_values = NULL;
//        o->extra = jarrobj_copy_data(src);
//    } else { // todo 其他情况，异常对象的情况
//        size_t len = sizeof(struct slot) * o->instance_fields_count;
//        o->instance_fields_values = malloc(len);
//        CHECK_MALLOC_RESULT(o->instance_fields_values);
//        memcpy(o->instance_fields_values, src->instance_fields_values, len);
//        o->extra = src->extra;
//    }
//    printvm("create object: %s\n", jobject_to_string(o));
//    return o;
}

bool object_is_jlstring(const struct object *o)
{
    assert(o != NULL);
    return strcmp(o->clazz->class_name, STR) == 0;
}

bool object_is_jlclass(const struct object *o)
{
    assert(o != NULL);
    return strcmp(o->clazz->class_name, CLS) == 0;
}

void set_instance_field_value(struct object *o, struct field *f, const slot_t *value)
{
//    o->data[id] = *value;
//    if (slot_is_category_two(value)) {
//        assert(id + 1 < o->clazz->instance_fields_count);
//        o->data[id + 1] = phslot;
//    }
    assert(o != NULL && f != NULL && value != NULL);

    o->data[f->id] =  value[0];
    if (f->category_two) {
        o->data[f->id + 1] = value[1];
    }
}

//void set_instance_field_value_by_nt(struct object *o, const char *name, const char *descriptor, const slot_t *value)
//{
//    assert(o != NULL && name != NULL && descriptor != NULL && value != NULL);
//
//    struct field *f = class_lookup_field(o->clazz, name, descriptor);
//    if (f == NULL) {
//        jvm_abort("error\n"); // todo
//    }
//
//    set_instance_field_value_by_id(o, f->id, value);
//}

const slot_t* get_instance_field_value_by_id(const struct object *o, int id)
{
    assert(o != NULL);
    assert(0 <= id && id < o->clazz->instance_fields_count);
    return o->data + id;
}

const slot_t* get_instance_field_value(const struct object *o, struct field *f)
{
    assert(o != NULL);
    assert(f != NULL);
    return o->data + f->id;
}

const slot_t* get_instance_field_value_by_nt(const struct object *o, const char *name, const char *descriptor)
{
    assert(o != NULL && name != NULL && descriptor != NULL);

    struct field *f = class_lookup_field(o->clazz, name, descriptor);
    if (f == NULL) {
        jvm_abort("error, %s, %s\n", name, descriptor); // todo
    }

    return get_instance_field_value(o, f);
}

bool object_is_instance_of(const struct object *o, const struct class *c)
{
    if (o == NULL || c == NULL)  // todo
        return false;
    return class_is_subclass_of(o->clazz, c);
}

const slot_t* priobj_unbox(const struct object *po)
{
    assert(po != NULL);
    assert(is_primitive(po->clazz));

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    return get_instance_field_value_by_nt(po, "value", po->clazz->class_name);
}

void object_destroy(struct object *o)
{
    if (o == NULL) {
        // todo
    }

    // todo
}

const char* object_to_string(const struct object *o)
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

#undef MAX_LEN
}
