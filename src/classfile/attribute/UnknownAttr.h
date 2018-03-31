/*
 * Author: Jia Yang
 */

#ifndef JVM_UNKNOWNATTR_H
#define JVM_UNKNOWNATTR_H


#include "Attribute.h"

// 不认识的attr，忽略之。
struct UnknownAttr: Attribute {
    explicit UnknownAttr(BytecodeReader &reader): Attribute(reader) {
        reader.skip(attributeLength); // todo
    }
};

#endif //JVM_UNKNOWNATTR_H
