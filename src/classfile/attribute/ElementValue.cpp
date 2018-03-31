/*
 * Author: Jia Yang
 */

#include "ElementValue.h"

ElementValue::ElementValue(BytecodeReader &reader) {
    tag = reader.readu1();
    switch (tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'S':
        case 'Z':
        case 's':
            value.constValueIndex = reader.readu2();
            break;
        case 'c':
            value.classInfoIndex = reader.readu2();
            break;
        case 'e':
            value.enumConstValue.typeNameIndex = reader.readu2();
            value.enumConstValue.constNameIndex = reader.readu2();
            break;
        case '@':
            value.annotationValue = new Annotation(reader);
            break;
        case '[':
            value.arrayValue.numValues = reader.readu2();
            value.arrayValue.values = new ElementValue[value.arrayValue.numValues];
            for (int i = 0; i < value.arrayValue.numValues; i++) {
                new (value.arrayValue.values + i)ElementValue(reader);
            }
            break;
        default:
            jvmAbort("error\n"); // todo error
    }
}


