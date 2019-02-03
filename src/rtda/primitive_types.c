/*
 * Author: Jia Yang
 */

#include <string.h>
#include "primitive_types.h"
#include "ma/class.h"
#include "../symbol.h"

/*
 * 基本类型的名称，描述符，等等
 */
static const struct {
    const char *class_name;
    char descriptor;
    const char *array_class_name;
    const char *wrapper_class_name;
} primitive_types[] = {
        { "void",    'V', "[V", "java/lang/Void" },
        { "boolean", 'Z', "[Z", "java/lang/Boolean" },
        { "byte",    'B', "[B", "java/lang/Byte" },
        { "char",    'C', "[C", "java/lang/Character" },
        { "short",   'S', "[S", "java/lang/Short" },
        { "int",     'I', "[I", "java/lang/Integer" },
        { "long",    'J', "[J", "java/lang/Long" },
        { "float",   'F', "[F", "java/lang/Float" },
        { "double",  'D', "[D", "java/lang/Double" },
};

#define PRIMITIVE_TYPE_COUNT (sizeof(primitive_types) / sizeof(*primitive_types))

void load_primitive_types()
{
    // 加载基本类型（int, float, etc.）的 class
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        struct class *c = class_create_primitive_class(g_bootstrap_loader, primitive_types[i].class_name);
        classloader_put_to_pool(g_bootstrap_loader, primitive_types[i].class_name, c);
    }
}

bool pt_is_primitive_class_name(const char *class_name)
{
    assert(class_name != NULL);
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].class_name, class_name) == 0) {
            return true;
        }
    }
    return false;
}

bool pt_is_primitive_descriptor(char descriptor)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (primitive_types[i].descriptor == descriptor) {
            return true;
        }
    }
    return false;
}

bool pt_is_primitive_wrapper_class_name(const char *wrapper_class_name)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].wrapper_class_name, wrapper_class_name) == 0) {
            return true;
        }
    }
    return false;
}

const char* pt_get_array_class_name_by_class_name(const char *class_name)
{
    assert(class_name != NULL);

    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].class_name, class_name) == 0) {
            return primitive_types[i].array_class_name;
        }
    }
    return NULL;
}

const char* pt_get_class_name_by_descriptor(char descriptor)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (primitive_types[i].descriptor == descriptor) {
            return primitive_types[i].class_name;
        }
    }
    return NULL;
}

char pt_get_descriptor_by_class_name(const char *class_name)
{
    assert(class_name != NULL);

    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].class_name, class_name) == 0) {
            return primitive_types[i].descriptor;
        }
    }

    return 0;
}
