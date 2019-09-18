/*
 * Author: kayo
 */

#include "ArrayClass.h"
#include "PrimitiveClass.h"

ArrayClass::ArrayClass(const char *className): Class(bootClassLoader, strdup(className))
{
    assert(className != nullptr);
    assert(className[0] == '[');

    accessFlags = ACC_PUBLIC;
    inited = true; // 数组类不需要初始化
    pkgName = "";
    superClass = java_lang_Object_class;
    interfaces.push_back(java_lang_Cloneable_class);
    interfaces.push_back(java_io_Serializable_class);

    createVtable();

    if (java_lang_Class_class != nullptr) {
        clazz = java_lang_Class_class;
        data = new slot_t[java_lang_Class_class->instFieldsCount]; // todo
    }
}

size_t ArrayClass::getEleSize()
{
    if (eleSize == 0) {
        // 判断数组单个元素的大小
        // 除了基本类型的数组外，其他都是引用类型的数组
        // 多维数组是数组的数组，也是引用类型的数组
        eleSize = sizeof(jref);
        char t = className[1]; // jump '['
        if (t == 'Z') { eleSize = sizeof(jbool); }
        else if (t == 'B') { eleSize = sizeof(jbyte); }
        else if (t == 'C') { eleSize = sizeof(jchar); }
        else if (t == 'S') { eleSize = sizeof(jshort); }
        else if (t == 'I') { eleSize = sizeof(jint); }
        else if (t == 'F') { eleSize = sizeof(jfloat); }
        else if (t == 'J') { eleSize = sizeof(jlong); }
        else if (t == 'D') { eleSize = sizeof(jdouble); }
    }

    return eleSize;
}

Class *ArrayClass::componentClass()
{
    if (compClass != nullptr)
        return compClass;

    const char *compName = className;
    for (; *compName == '['; compName++); // jump all '['

    if (*compName != 'L') { // primitive type
        compClass = getPrimitiveClass(*compName);
        assert(compClass != nullptr);
        return compClass;
    }

    compName++;
    int last = strlen(compName) - 1;
    assert(last > 0);
    if (compName[last] != ';') {
        raiseException(UNKNOWN_ERROR, className); // todo
    } else {
        char buf[last + 1];
        strncpy(buf, compName, (size_t) last);
        buf[last] = 0;
        compClass = loader->loadClass(buf);
        return compClass;
    }
}