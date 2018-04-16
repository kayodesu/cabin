/*
 * Author: Jia Yang
 */

#include <cassert>
#include "ClassFile.h"


static void parseMemberInfo(BytecodeReader &reader, Constant **constantPool, MemberInfo *info) {
    info->accessFlags = reader.readu2();
    info->nameIndex = reader.readu2();
    info->descriptorIndex = reader.readu2();
    info->attributesCount = reader.readu2();
    info->attributes = new Attribute* [info->attributesCount];
    for (int i = 0; i < info->attributesCount; i++) {
        info->attributes[i] = Attribute::parseAttr(reader, constantPool);
    }
}

ClassFile::ClassFile(const s1 *bytecode, size_t len) {
    BytecodeReader reader(bytecode, len);

    magic = reader.readu4();
    minorVersion = reader.readu2();
    majorVersion = reader.readu2();

    // parse constant pool
    constantPoolCount = reader.readu2();
    constantPool = new Constant* [constantPoolCount];
    constantPool[0] = nullptr; // constant pool 从 1 开始计数
    for (int i = 1; i < constantPoolCount; i++) {
        constantPool[i] = Constant::parseConstant(reader);

        // 在Class文件的常量池中，所有的8字节的常量都占两个表成员（项）的空间。
        // 如果一个CONSTANT_Long_info或CONSTANT_Double_info结构的项在常量池中的索引为n，
        // 则常量池中下一个有效的项的索引为n+2，此时常量池中索引为n+1的项有效但必须被认为不可用
        u1 tag = constantPool[i]->tag;
        if (tag == LONG_CONSTANT || tag == DOUBLE_CONSTANT) {
            i++;   // todo
            constantPool[i] = nullptr;
        }
    }

    accessFlags = reader.readu2();
    thisClass = reader.readu2();
    superClass = reader.readu2();

    // parse interfaces
    interfacesCount = reader.readu2();
    interfaces = new u2[interfacesCount];
    for (int i = 0; i < interfacesCount; i++) {
        interfaces[i] = reader.readu2();
    }

    // parse fields
    fieldsCount = reader.readu2();
    fields = new MemberInfo[fieldsCount];
    for (int i = 0; i < fieldsCount; i++) {
        parseMemberInfo(reader, constantPool, fields + i);
    }

    // parse methods
    methodsCount = reader.readu2();
    methods = new MemberInfo[methodsCount];
    for (int i = 0; i < methodsCount; i++) {
        parseMemberInfo(reader, constantPool, methods + i);
    }

    // parse class attributes
    attributesCount = reader.readu2();
    attributes = new Attribute* [attributesCount];
    for (int i = 0; i < attributesCount; i++) {
        attributes[i] = Attribute::parseAttr(reader, constantPool);
    }
}

ClassFile::ClassFile(const char *classfilePath) {
    assert(classfilePath != nullptr);

    FILE *f = fopen(classfilePath, "rb");
    if (f == NULL) {
        jprintf("open file failed: %s\n", classfilePath); // todo
//        return NULL; // todo
    }

    fseek(f, 0, SEEK_END); //定位到文件末
    size_t file_len = (size_t) ftell(f); //文件长度 // todo
    if (file_len == -1) {
        jprintf("error\n"); // todo
//        return NULL; // todo
    }

    s1 *bytecode = new s1[file_len];

    fseek(f, 0, SEEK_SET);

    fread(bytecode, 1, file_len, f);
    fclose(f);

    new (this)ClassFile(bytecode, file_len);
}

