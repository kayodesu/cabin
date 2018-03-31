/*
 * Author: Jia Yang
 */

#ifndef JVM_SOURCEFILEATTR_H
#define JVM_SOURCEFILEATTR_H

#include "Attribute.h"

struct SourceFileAttr: public Attribute {
    u2 sourcefileIndex; // 常量池索引， 指向CONSTANT_Utf8_info常量。
    explicit SourceFileAttr(BytecodeReader &reader): Attribute(reader) {
        sourcefileIndex = reader.readu2();
    }
};

#endif //JVM_SOURCEFILEATTR_H
