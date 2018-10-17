/*
 * Author: Jia Yang
 */

#ifndef JVM_PRIMITIVETYPES_H
#define JVM_PRIMITIVETYPES_H


#include <stdbool.h>
#include <string.h>

/*
 * todo 这个类是干嘛的
 */
static const struct {
    const char *name;
    const char *descriptor;
    const char *array_class_name;
    const char *wrapper_class_name;
} primitive_types[] = {
    { "void",    "V", "[V", "java/lang/Void" },
    { "boolean", "Z", "[Z", "java/lang/Boolean" },
    { "byte",    "B", "[B", "java/lang/Byte" },
    { "char",    "C", "[C", "java/lang/Character" },
    { "short",   "S", "[S", "java/lang/Short" },
    { "int",     "I", "[I", "java/lang/Integer" },
    { "long",    "J", "[J", "java/lang/Long" },
    { "float",   "F", "[F", "java/lang/Float" },
    { "double",  "D", "[D", "java/lang/Double" },
};

#define PRIMITIVE_TYPE_COUNT (sizeof(primitive_types) / sizeof(*primitive_types))

static bool is_primitive_by_class_name(const char *class_name)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++) {
        if (strcmp(primitive_types[i].wrapper_class_name, class_name) == 0) {
            return true;
        }
    }
    return false;
}


#endif //JVM_PRIMITIVETYPES_H
