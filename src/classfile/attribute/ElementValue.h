/*
 * Author: Jia Yang
 */

#ifndef JVM_ELEMENTVALUE_H
#define JVM_ELEMENTVALUE_H

#include "../../jvmtype.h"
#include "Annotation.h"
#include "../../BytecodeReader.h"

struct Annotation;

/*
 The tag item uses a single ASCII character to indicate the type of the value of
 the element-value pair. This determines which item of the value union is in use.

 Interpretation of tag values as types
 tag Item    Type           value Item               constant Type
 ----------------------------------------------------------------------
 B          byte          const_value_index           CONSTANT_Integer
 C          char          const_value_index           CONSTANT_Integer
 D          double        const_value_index           CONSTANT_Double
 F          float         const_value_index           CONSTANT_Float
 I          int           const_value_index           CONSTANT_Integer
 J          long          const_value_index           CONSTANT_Long
 S          short         const_value_index           CONSTANT_Integer
 Z          boolean       const_value_index           CONSTANT_Integer
 s          String        const_value_index           CONSTANT_Utf8
 e          Enum type     enum_const_value            Not applicable
 c          Class         class_info_index            Not applicable
 @          Annotation    type annotation_value       Not applicable
 [          Array type    array_value                 Not applicable
 */
struct ElementValue {
    u1 tag;

    struct EnumConstValue {
        u2 typeNameIndex;
        u2 constNameIndex;
    };

    struct ArrayValue {
        u2 numValues;
        ElementValue *values; // [num_values];
    };

    union {
        u2 constValueIndex;
        EnumConstValue enumConstValue;
        u2 classInfoIndex;
        Annotation *annotationValue;
        ArrayValue arrayValue;
    } value;

    ElementValue() {}
    explicit ElementValue(BytecodeReader &reader);
};

#endif //JVM_ELEMENTVALUE_H
