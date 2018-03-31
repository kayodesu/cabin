/*
 * Author: Jia Yang
 */


#include "../../BytecodeReader.h"
#include "Annotation.h"


Annotation::Annotation(BytecodeReader &reader) {
    typeIndex = reader.readu2();
    numElementValuePairs = reader.readu2();

    elementValuePairs = new ElementValuePair[numElementValuePairs];
    for (int i = 0; i < numElementValuePairs; i++) {
        elementValuePairs[i].elementNameIndex = reader.readu2();
        elementValuePairs[i].value = new ElementValue(reader);
    }
}