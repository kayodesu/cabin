#include "jobject.h"

/*
 * Author: Jia Yang
 */

struct slot jpriobj_unbox(const struct jobject *po)
{
    assert(po != NULL);
    assert(jobject_is_primitive(po));

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    return *get_instance_field_value_by_nt(po, "value", po->jclass->class_name);
}
