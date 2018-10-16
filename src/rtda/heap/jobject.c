/*
 * Author: Jia Yang
 */

#include "jobject.h"
#include "mm/halloc.h"
#include "../ma/jclass.h"
#include "../ma/jfield.h"

struct jobject* jobject_create(struct jclass *c)
{
    if (c == NULL) {
        jvm_abort("jclass is null");  // todo
        return NULL;
    }

    if (is_array(c)) {
        // todo
        return NULL;
    }

//    struct jobject *o = halloc(sizeof(*o));
    HEAP_ALLOC(struct jobject, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    o->jclass = c;
    o->instance_fields_count = c->instance_fields_count;
    o->instance_field_values = fv_create(o->jclass, o->instance_fields_count);//malloc(sizeof(struct slot) * o->instance_fields_count);

    return o;
}

void jobject_destroy(struct jobject *o)
{
    if (o == NULL) {
        // todo
    }

    // todo

    hfree(o);
}

bool jobject_is_instance_of(const struct jobject *o, const struct jclass *c)
{
    if (o == NULL || c == NULL)  // todo
        return false;
    return jclass_is_subclass_of(o->jclass, c);
}

//void jobject_set_field_value(struct jobject *o, int id, const struct slot *v)
//{
//    assert(o != NULL && v != NULL);
//    assert(id >= 0 && id < o->instance_fields_count);
//
//    o->instance_fields_values[id] = *v;
//    if (slot_is_category_two(v)) {
//        assert(id + 1 < o->instance_fields_count);
//        o->instance_fields_values[id + 1] = phslot();
//    }
//}

//void jobject_set_field_value_nt(struct jobject *o, const char *name, const char *descriptor, struct slot *v)
//{
//    assert(o != NULL && name != NULL && descriptor != NULL && v != NULL);
//
//    struct jfield *field = jclass_lookup_field(o->jclass, name, descriptor);
//    if (field == NULL) {
//        // todo
//        jvm_abort("%s, %s", name, descriptor);
//    }
//    jobject_set_field_value(o, field->id, v);
//}

//struct slot* jobject_instance_field_value(struct jobject *o, int id)
//{
//    assert(o != NULL);
//    assert(id >= 0 && id < o->instance_fields_count);
//    return o->instance_fields_values + id;
//}
//
//struct slot* jobject_instance_field_value_nt(struct jobject *o, const char *name, const char *descriptor)
//{
//    assert(o != NULL && name != NULL && descriptor != NULL);
//
//    struct jfield *field = jclass_lookup_instance_field(o->jclass, name, descriptor);
//    if (field == NULL) {
//        // todo
//        jvm_abort("%s, %s", name, descriptor);
//    }
//    return &field->value;
////    return jobject_instance_field_value(o, field->id);
//}
