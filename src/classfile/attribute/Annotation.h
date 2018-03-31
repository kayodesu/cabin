/*
 * Author: Jia Yang
 */

#ifndef JVM_ANNOTATION_H
#define JVM_ANNOTATION_H

#include "ElementValue.h"
#include "../../BytecodeReader.h"

struct ElementValue;

class Annotation {
public:
    u2 typeIndex;
    u2 numElementValuePairs;

    struct ElementValuePair {
        u2 elementNameIndex;
        ElementValue *value;
    };

    ElementValuePair *elementValuePairs; // [num_element_value_pairs];

    Annotation() {}

    Annotation(BytecodeReader &reader);
};


#endif //JVM_ANNOTATION_H
