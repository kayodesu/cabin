/*
 * Author: kayo
 */

#include <cstring>
#include "primitive_types.h"
#include "ma/Class.h"

/*
 * 基本类型的名称，描述符，等等
 */
static const struct {
    const char *class_name;
    char descriptor;
    const char *array_class_name;
    const char *wrapper_class_name;
} primitiveTypes[] = {
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

void loadPrimitiveTypes()
{
    // 加载基本类型（int, float, etc.）的 class
    for (auto t : primitiveTypes) {
        Class *c = new PrimitiveClass(t.class_name);
        bootClassLoader->putToPool(t.class_name, c);
    }
}

bool isPrimitiveClassName(const char *class_name)
{
    assert(class_name != nullptr);
    for (auto t : primitiveTypes) {
        if (strcmp(t.class_name, class_name) == 0)
            return true;
    }
    return false;
}

bool isPrimitiveDescriptor(char descriptor)
{
    for (auto t : primitiveTypes) {
        if (t.descriptor == descriptor)
            return true;
    }
    return false;
}

const char *primitiveClassName2arrayClassName(const char *class_name)
{
    assert(class_name != nullptr);

    for (auto t : primitiveTypes) {
        if (strcmp(t.class_name, class_name) == 0)
            return t.array_class_name;
    }
    return nullptr;
}

const char *primitiveDescriptor2className(char descriptor)
{
    for (auto t : primitiveTypes) {
        if (t.descriptor == descriptor)
            return t.class_name;
    }
    return nullptr;
}

char primitiveClassName2descriptor(const char *class_name)
{
    assert(class_name != nullptr);

    for (auto t : primitiveTypes) {
        if (strcmp(t.class_name, class_name) == 0)
            return t.descriptor;
    }

    return 0;
}
