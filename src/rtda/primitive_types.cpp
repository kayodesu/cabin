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
    PrimitiveClass *clazz;
} primitiveTypes[] = {
        { "void",    'V', "[V", "java/lang/Void",      nullptr },
        { "boolean", 'Z', "[Z", "java/lang/Boolean",   nullptr },
        { "byte",    'B', "[B", "java/lang/Byte",      nullptr },
        { "char",    'C', "[C", "java/lang/Character", nullptr },
        { "short",   'S', "[S", "java/lang/Short",     nullptr },
        { "int",     'I', "[I", "java/lang/Integer",   nullptr },
        { "long",    'J', "[J", "java/lang/Long",      nullptr },
        { "float",   'F', "[F", "java/lang/Float",     nullptr },
        { "double",  'D', "[D", "java/lang/Double",    nullptr },
};

void loadPrimitiveTypes()
{
    assert(bootClassLoader != nullptr);
    // 加载基本类型（int, float, etc.）的 class
    for (auto t : primitiveTypes) {
        t.clazz = new PrimitiveClass(t.class_name);
        bootClassLoader->putToPool(t.class_name, t.clazz);
    }
}

PrimitiveClass *getPrimitiveClass(const char *className)
{
    for (auto t : primitiveTypes) {
        if (strcmp(t.class_name, className) == 0)
            return t.clazz;
    }
    return nullptr;
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
