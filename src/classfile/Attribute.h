/*
 * Author: kayo
 */

#ifndef JVM_ATTRIBUTE_H
#define JVM_ATTRIBUTE_H

#include <vector>
#include "../jtypes.h"

struct ElementValuePair;
class BytecodeReader;

struct Annotation {
    // The value of the type_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure representing a Field descriptor.
    // The Field descriptor denotes the type of the annotation represented  by this annotation structure.
    u2 type_index = 0;
    std::vector<ElementValuePair> elementValuePairs;

    Annotation() = default;

    explicit Annotation(BytecodeReader &r) { read(r); }

    void read(BytecodeReader &r);
};

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

   // union {
        u2 const_value_index;
        u2 class_info_index;
        struct {
            u2 type_name_index;
            u2 const_name_index;
        } enum_const_value;

        Annotation annotation_value;

        std::vector<ElementValue> arrayValue;
 //   };

    ElementValue() = default;

    explicit ElementValue(BytecodeReader &r) { read(r); }

    void read(BytecodeReader &r);
};

struct ElementValuePair {
    // The value of the element_name_index item must be a valid index into the constant_pool table.
    // The constant_pool entry at that index must be a CONSTANT_Utf8_info structure.
    // The constant_pool entry denotes the name of the element of the element-value pair
    // represented by this element_value_pairs entry.
    // In other words, the entry denotes an element of the annotation type specified by type_index.
    u2 element_name_index;
    ElementValue value;

    explicit ElementValuePair(BytecodeReader &r);
};


#endif //JVM_ATTRIBUTE_H
