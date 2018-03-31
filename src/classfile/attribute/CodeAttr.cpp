/*
 * Author: Jia Yang
 */

#include "CodeAttr.h"

CodeAttr::CodeAttr(BytecodeReader &reader, Constant **constantPool): Attribute(reader) {
    maxStack = reader.readu2();
    maxLocals = reader.readu2();
    codeLength = reader.readu4();
    code = new u1[codeLength];//malloc(sizeof(u1) * code_length);
    for (int i = 0; i < codeLength; i++) {
        code[i] = reader.readu1();
    }

    exceptionTableLength = reader.readu2();
    exceptionTables = new ExceptionTable[exceptionTableLength];
    for (int i = 0; i < exceptionTableLength; i++) {
        exceptionTables[i].startPc = reader.readu2();
        exceptionTables[i].endPc = reader.readu2();
        exceptionTables[i].handlerPc = reader.readu2();
        exceptionTables[i].catchType = reader.readu2();
    }

    attributesCount = reader.readu2();
    attributes = new Attribute* [attributesCount];
    for (int i = 0; i < attributesCount; i++) {
        attributes[i] = Attribute::parseAttr(reader, constantPool);
    }
}