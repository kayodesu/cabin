/*
 * Author: Jia Yang
 */

#include <string.h>
#include <stdlib.h>
#include "jobject.h"
#include "mm/halloc.h"
#include "../ma/jfield.h"

static inline struct jobject* jobject_alloc()
{
    HEAP_ALLOC(struct jobject, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    return o;
}

struct jobject* jobject_create(struct jclass *c)
{
    assert(c != NULL);

    struct jobject *o = jobject_alloc();
    o->jclass = c;

    if (jclass_is_array(c)) {  // todo
        o->instance_fields_count = 0;
        o->instance_fields_values = NULL;
    } else {
        o->instance_fields_count = c->instance_fields_count;
        o->instance_fields_values = copy_inited_instance_fields_values(c);
    }

    o->extra = NULL;
    return o;
}

void* jarrobj_copy_data(const struct jobject *o);

struct jobject* jobject_clone(const struct jobject *src, struct jobject *dest)
{
    assert(src != NULL);
    struct jobject *o = dest != NULL ? dest : jobject_alloc();

    o->jclass = src->jclass;
    o->instance_fields_count = src->instance_fields_count;

    if (jobject_is_array(src)) {
        o->instance_fields_values = NULL;
        o->extra = jarrobj_copy_data(src);
    } else { // todo 其他情况，异常对象的情况
        int len = sizeof(struct slot) * o->instance_fields_count;
        o->instance_fields_values = malloc(len);
        memcpy(o->instance_fields_values, src->instance_fields_values, len);
        o->extra = src->extra;
    }
    printvm("create object: %s\n", jobject_to_string(o));
    return o;
}

bool jobject_is_array(const struct jobject *o)
{
    assert(o != NULL);
    return jclass_is_array(o->jclass);
}

bool jobject_is_jlstring(const struct jobject *o)
{
    assert(o != NULL);
    return strcmp(o->jclass->class_name, "java/lang/String") == 0;
}

bool jobject_is_jlclass(const struct jobject *o)
{
    assert(o != NULL);
    return strcmp(o->jclass->class_name, "java/lang/Class") == 0;
}

void set_instance_field_value_by_id(const struct jobject *o, int id, const struct slot *value)
{
    assert(o != NULL && value != NULL);
    assert(id >= 0 && id < o->instance_fields_count);
    o->instance_fields_values[id] = *value;
    if (slot_is_category_two(value)) {
        assert(id + 1 < o->instance_fields_count);
        o->instance_fields_values[id + 1] = phslot;
    }
}

void set_instance_field_value_by_nt(const struct jobject *o,
                                  const char *name, const char *descriptor, const struct slot *value)
{
    assert(o != NULL && name != NULL && descriptor != NULL && value != NULL);

    struct jfield *f = jclass_lookup_field(o->jclass, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    set_instance_field_value_by_id(o, f->id, value);
}

const struct slot* get_instance_field_value_by_id(const struct jobject *o, int id)
{
    assert(o != NULL);
    assert(id >= 0 && id < o->instance_fields_count);
    return o->instance_fields_values + id;
}

const struct slot* get_instance_field_value_by_nt(const struct jobject *o, const char *name, const char *descriptor)
{
    assert(o != NULL && name != NULL && descriptor != NULL);

    struct jfield *f = jclass_lookup_field(o->jclass, name, descriptor);
    if (f == NULL) {
        jvm_abort("error\n"); // todo
    }

    return get_instance_field_value_by_id(o, f->id);
}

bool jobject_is_instance_of(const struct jobject *o, const struct jclass *c)
{
    if (o == NULL || c == NULL)  // todo
        return false;
    return jclass_is_subclass_of(o->jclass, c);
}

void jobject_destroy(struct jobject *o)
{
    if (o == NULL) {
        // todo
    }

    // todo

    hfree(o);
}

const char* jobject_to_string(const struct jobject *o)
{
#define MAX_LEN 1023 // big enough? todo
    VM_MALLOCS(char, MAX_LEN + 1, result);
    if (o == NULL) {
        strcpy(result, "object is null");
        return result;
    }

    int n = snprintf(result, MAX_LEN, "object(%p), %s", o, o->jclass->class_name);
    // todo extra
    assert(0 <= n && n <= MAX_LEN);
    result[n] = 0;
    return result;

#undef MAX_LEN
}
