/*
 * Author: Jia Yang
 */

#ifndef JVM_SIGNATUREATTR_H
#define JVM_SIGNATUREATTR_H

#include "Attribute.h"

/*
 * The Signature attribute is a fixed-length attribute in the attributes table
of a ClassFile, field_info, or method_info structure (§4.1, §4.5, §4.6). A
Signature attribute records a signature (§4.7.9.1) for a class, interface, constructor,
method, or field whose declaration in the Java programming language uses type
variables or parameterized types. See The Java Language Specification, Java SE
8 Edition for details about these types.
 */
struct SignatureAttr: public Attribute {
    /*
     * The value of the signature_index item must be a valid index into the
constant_pool table. The constant_pool entry at that index must be a
CONSTANT_Utf8_info structure (§4.4.7) representing a class signature if this
Signature attribute is an attribute of a ClassFile structure; a method
signature if this Signature attribute is an attribute of a method_info structure;
or a field signature otherwise.
     */
    u2 signatureIndex;

    explicit SignatureAttr(BytecodeReader &reader): Attribute(reader) {
        signatureIndex = reader.readu2();
    }
};

#endif //JVM_SIGNATUREATTR_H
