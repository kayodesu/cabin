/*
 * Author: Jia Yang
 */

#ifndef JVM_BOOTSTRAPMETHODSATTR_H
#define JVM_BOOTSTRAPMETHODSATTR_H

#include "Attribute.h"

/*
 * The BootstrapMethods attribute is a variable-length attribute in the attributes
 * table of a ClassFile structure. The BootstrapMethods attribute
 * records bootstrap method specifiers referenced by invokedynamic instructions.
 * There must be exactly one BootstrapMethods attribute in the attributes table of
 * a ClassFile structure if the constant_pool table of the ClassFile structure has
 * at least one CONSTANT_InvokeDynamic_info entry.
 * There may be at most one BootstrapMethods attribute in the attributes table of
 * a ClassFile structure.
 */
struct BootstrapMethodsAttr: public Attribute {
    u2 numBootstrapMethods;

    struct BootstrapMethod {
        u2 bootstrapMethodRef;
        u2 numBootstrapArguments;
        u2 *bootstrapArguments; // num_bootstrap_arguments
    };

    BootstrapMethod *bootstrapMethods;

    explicit BootstrapMethodsAttr(BytecodeReader &reader): Attribute(reader) {
        numBootstrapMethods = reader.readu2();
        bootstrapMethods = new BootstrapMethod[numBootstrapMethods];
        for (int i = 0; i < numBootstrapMethods; i++) {
            auto &tmp = bootstrapMethods[i];
            tmp.bootstrapMethodRef = reader.readu2();
            tmp.numBootstrapArguments = reader.readu2();
            tmp.bootstrapArguments = new u2[tmp.numBootstrapArguments];
            for (int j = 0; j < tmp.numBootstrapArguments; j++) {
                tmp.bootstrapArguments[j] = reader.readu2();
            }
        }
    }

    ~BootstrapMethodsAttr() {
        // todo
    }
};

#endif //JVM_BOOTSTRAPMETHODSATTR_H
