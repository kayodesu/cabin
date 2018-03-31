/*
 * Author: Jia Yang
 */

#ifndef JVM_CONSTANTVALUEATTR_H
#define JVM_CONSTANTVALUEATTR_H

#include "Attribute.h"

/*
 * The ConstantValue attribute is a fixed-length attribute in the attributes table of
 * a field_info structure. A ConstantValue attribute represents the value of
 * a constant expression, and is used as follows:
 * If the ACC_STATIC flag in the access_flags item of the field_info structure is
 * set, then the field represented by the field_info structure is assigned the value
 * represented by its ConstantValue attribute as part of the initialization of the
 * class or interface declaring the field. This occurs prior to the invocation
 * of the class or interface initialization method of that class or interface.
 *
 * Otherwise, the Java Virtual Machine must silently ignore the attribute.
 *
 * The value of the constantvalue_index item must be a valid index into
 * the constant_pool table. The constant_pool entry at that index gives the
 * constant value represented by this attribute. The constant_pool entry must be
 * of a type appropriate to the field.
    ------------------------------------------
    Constant value attribute types
    ------------------------------------------
    Field Type                      | Entry Type
    ------------------------------------------
    long                            | CONSTANT_Long
    float                           | CONSTANT_Float
    double                          | CONSTANT_Double
    int, short, char, byte, boolean | CONSTANT_Integer
    String                          | CONSTANT_String
    ------------------------------------------
 */
struct ConstantValueAttr: public Attribute {
    u2 constantvalueIndex;

    explicit ConstantValueAttr(BytecodeReader &reader): Attribute(reader) {
        constantvalueIndex = reader.readu2();
    }
};

#endif //JVM_CONSTANTVALUEATTR_H
