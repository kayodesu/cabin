#include <string.h>
#include <assert.h>
#include "jtypes.h"
#include "jvm.h"

/*
 * Author: Jia Yang
 */

const char* get_jtype_name(enum jtype t)
{
    static const char* names[] = {
        "jbyte", "jbool", "jchar", "jshort", "jint",
        "jlong", "jfloat", "jdouble", "jref", "placeholder", "not a tpe"
    };

    int len = sizeof(names) / sizeof(*names);
    if (t < 0 || t >= len) {
        printvm("access beyoud bounds, %d of %d\n", t, len); // todo
        return "unknown";
    }

    return names[t];
}

bool is_primitive_type_descriptor(char descriptor)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (primitive_types[i].descriptor == descriptor) {
            return true;
        }
    }
    return false;
}

bool is_primitive_by_class_name(const char *class_name)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].wrapper_class_name, class_name) == 0) {
            return true;
        }
    }
    return false;
}

const char* primitive_type_get_array_class_name_by_class_name(const char *class_name)
{
    assert(class_name != NULL);

    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].name, class_name) == 0) {
            return primitive_types[i].array_class_name;
        }
    }
    return NULL;
}

const char* primitive_type_get_primitive_name_by_descriptor(char descriptor)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (primitive_types[i].descriptor == descriptor) {
            return primitive_types[i].name;
        }
    }
    return NULL;
}
