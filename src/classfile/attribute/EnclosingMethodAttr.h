/*
 * Author: Jia Yang
 */

#ifndef JVM_ENCLOSINGMETHODATTR_H
#define JVM_ENCLOSINGMETHODATTR_H

#include "Attribute.h"

/*
 * The EnclosingMethod attribute is a fixed-length attribute in the attributes table
of a ClassFile structure (§4.1). A class must have an EnclosingMethod attribute
if and only if it represents a local class or an anonymous class (JLS §14.3, JLS
§15.9.5).
There may be at most one EnclosingMethod attribute in the attributes table of
a ClassFile structure.
 */
struct EnclosingMethodAttr: public Attribute {
    /*
     * The value of the class_index item must be a valid index into the
constant_pool table. The constant_pool entry at that index must be a
CONSTANT_Class_info structure (§4.4.1) representing the innermost class that
encloses the declaration of the current class.
     */
    u2 classIndex;
    /*
     * If the current class is not immediately enclosed by a method or constructor,
then the value of the method_index item must be zero.
Otherwise, the value of the method_index item must be a valid index into
the constant_pool table. The constant_pool entry at that index must be a
CONSTANT_NameAndType_info structure (§4.4.6) representing the name and
type of a method in the class referenced by the class_index attribute above.
     */
    u2 methodIndex;

    explicit EnclosingMethodAttr(BytecodeReader &reader): Attribute(reader) {
        classIndex = reader.readu2();
        methodIndex = reader.readu2();
    }
};

#endif //JVM_ENCLOSINGMETHODATTR_H
