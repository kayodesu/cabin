/*
 * Author: Jia Yang
 */

#include "jclassobj.h"

struct jclassobj* jclassobj_create(struct jclass *jclass_class, const char *class_name)
{
    HEAP_ALLOC(struct jclassobj, co);

    co->obj = jobject_create(jclass_class);
    co->class_name = strdup(class_name);

    return co;
}

void jclassobj_destroy(struct jclassobj *co)
{
    if (co == NULL) {
        // todo
        return;
    }

    free(co->class_name);
    hfree(co);
}
