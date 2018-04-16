/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASSFILE_H
#define JVM_CLASSFILE_H

#include <vector>
#include "../jvm.h"
#include "attribute/Attribute.h"

struct MemberInfo {
    u2 accessFlags;
    u2 nameIndex;
    u2 descriptorIndex;
    u2 attributesCount;
    Attribute **attributes; // [attributes_count];
//    std::vector<Attribute *> attributes;
};

struct ClassFile {
    u4 magic;
    u2 minorVersion;
    u2 majorVersion;

    u2 constantPoolCount;
    Constant **constantPool; // The constant_pool table is indexed from 1 to constant_pool_count - 1.
//    std::vector<void *> constant_pool; // The constant_pool table is indexed from 1 to constant_pool_count - 1.

    u2 accessFlags;
    u2 thisClass;
    u2 superClass;

    u2 interfacesCount;
    u2 *interfaces; // [interfaces_count];
//    std::vector<u2> interfaces;

    u2 fieldsCount;
    MemberInfo *fields; // [fields_count];
//    std::vector<member_info> fileds;

    u2 methodsCount;
    MemberInfo *methods; // [methods_count];
//    std::vector<member_info> methods;

    u2 attributesCount;
    Attribute **attributes; // [attributes_count];
//    std::vector<Attribute *> attributes;

    ClassFile(const s1 *bytecode, size_t len);
    explicit ClassFile(const char *classfilePath);
};

#endif //JVM_CLASSFILE_H
