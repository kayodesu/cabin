/*
 * Author: Jia Yang
 */

#include "../jvm.h"
#include "Attribute.h"
#include "../util/BytecodeReader.h"

void Annotation::read(BytecodeReader &r)
{
    type_index = r.readu2();
    u2 num = r.readu2();
    for (u2 i = 0; i < num; i++) {
        elementValuePairs.emplace_back(ElementValuePair(r));
    }
}

ElementValue::ElementValue(BytecodeReader &r)
{
    tag = r.readu1();
    u2 num;
    switch (tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'S':
        case 'Z':
        case 's':
            const_value_index = r.readu2();
            break;
        case 'c':
            class_info_index = r.readu2();
            break;
        case 'e':
            enum_const_value.type_name_index = r.readu2();
            enum_const_value.const_name_index = r.readu2();
            break;
        case '@':
            annotation_value.read(r);
            break;
        case '[':
            num = r.readu2();
            for (u2 i = 0; i < num; i++) {
                arrayValue.emplace_back(ElementValue(r));
            }
            break;
        default:
            VM_UNKNOWN_ERROR("unknown tag: %d", tag);
    }
}

ElementValuePair::ElementValuePair(BytecodeReader &r): value(r)
{
    element_name_index = r.readu2();
}

BootstrapMethod::BootstrapMethod(BytecodeReader &r)
{
    bootstrapMethodRef = r.readu2();
    u2 num = r.readu2();
    for (u2 i = 0; i < num; i++) {
        bootstrapArguments.push_back(r.readu2());
    }
}

LineNumberTable::LineNumberTable(BytecodeReader &r)
{
    start_pc = r.readu2();
    line_number = r.readu2();;
}

ElementValue::~ElementValue()
{

}
