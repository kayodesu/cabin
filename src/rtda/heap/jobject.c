/*
 * Author: Jia Yang
 */

#include "jobject.h"
#include "mm/halloc.h"
#include "../ma/jclass.h"
#include "../ma/jfield.h"

struct jobject* jobject_create(struct jclass *c)
{
    assert(c != NULL);

    HEAP_ALLOC(struct jobject, o);
    if (o == NULL) {
        // todo 堆内存分配失败
        jvm_abort("堆溢出");
    }
    o->jclass = c;

    if (is_array(c)) {  // todo
        o->instance_fields_count = 0;
        o->instance_field_values = NULL;
    } else {
        o->instance_fields_count = c->instance_fields_count;
        o->instance_field_values = fv_create(o->jclass, o->instance_fields_count);
    }

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

