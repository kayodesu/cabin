/*
 * Author: kayo
 */

#ifndef KAYOVM_PRIMITIVECLASS_H
#define KAYOVM_PRIMITIVECLASS_H

#include "Class.h"

// 基本类型（int, float etc.）的 class.
class PrimitiveClass: public Class {
public:
    explicit PrimitiveClass(const char *className): Class(bootClassLoader, className)
    {
        assert(className != nullptr);
        accessFlags = ACC_PUBLIC;
        pkgName = "";
        inited = true;
        // todo super_class ???? java.lang.Object ??????
        superClass = java_lang_Object_class;

        createVtable();
    }
};

#endif //KAYOVM_PRIMITIVECLASS_H
