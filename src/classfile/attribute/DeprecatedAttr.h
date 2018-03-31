/*
 * Author: Jia Yang
 */

#ifndef JVM_DEPRECATEDATTR_H
#define JVM_DEPRECATEDATTR_H

#include "Attribute.h"

struct DeprecatedAttr: public Attribute {
    explicit DeprecatedAttr(BytecodeReader &reader): Attribute(reader){}
};

#endif //JVM_DEPRECATEDATTR_H
