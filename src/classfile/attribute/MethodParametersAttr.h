/*
 * Author: Jia Yang
 */

#ifndef JVM_METHODPARAMETERSATTR_H
#define JVM_METHODPARAMETERSATTR_H


#include "Attribute.h"

/*
 * The MethodParameters attribute is a variable-length attribute in the attributes
 * table of a method_info structure . A MethodParameters attribute records
 * information about the formal parameters of a method, such as their names.
 * There may be at most one MethodParameters attribute in the attributes table of a method_info structure.
 */
struct MethodParametersAttr: public Attribute {
    u1 parametersCount;
    struct Parameter {
        u2 nameIndex;
        u2 accessFlags;
    }; // [parameters_count];

    Parameter *parameters;

    explicit MethodParametersAttr(BytecodeReader &reader): Attribute(reader) {
        parametersCount = reader.readu1();
        parameters = new Parameter[parametersCount];//malloc(sizeof(*parameters) * parameters_count);
        for (int i = 0; i < parametersCount; i++) {
            parameters[i].nameIndex = reader.readu2();
            parameters[i].accessFlags = reader.readu2();
        }
    }
};


#endif //JVM_METHODPARAMETERSATTR_H
