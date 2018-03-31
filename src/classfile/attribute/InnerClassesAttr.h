/*
 * Author: Jia Yang
 */

#ifndef JVM_INNERCLASSESATTR_H
#define JVM_INNERCLASSESATTR_H

#include "Attribute.h"

struct InnerClassesAttr: public Attribute {
    u2 numberOfClasses;

    struct InnerClass {
        /*
         * The value of the inner_class_info_index item must be a valid index into
the constant_pool table. The constant_pool entry at that index must be
a CONSTANT_Class_info structure representing C. The remaining items in
the classes array entry give information about C.
         */
        u2 innerClassInfoIndex;
        /*
         * If C is not a member of a class or an interface (that is, if C is a top-level
class or interface (JLS §7.6) or a local class (JLS §14.3) or an anonymous
class (JLS §15.9.5)), the value of the outer_class_info_index item must
be zero.
Otherwise, the value of the outer_class_info_index item must be a valid
index into the constant_pool table, and the entry at that index must be
a CONSTANT_Class_info structure representing the class or interface of
which C is a member.
         */
        u2 outerClassInfoIndex;
        u2 innerNameIndex;
        /*
         * If C is anonymous (JLS §15.9.5), the value of the inner_name_index item
must be zero.
Otherwise, the value of the inner_name_index item must be a valid index
into the constant_pool table, and the entry at that index must be a
CONSTANT_Utf8_info structure (§4.4.7) that represents the original simple
name of C, as given in the source code from which this class file was
compiled.
         */
        u2 innerClassAccessFlags;
    };

    InnerClass *classes;  // [number_of_classes];

    explicit InnerClassesAttr(BytecodeReader &reader): Attribute(reader) {
        numberOfClasses = reader.readu2();
        classes = new InnerClass[numberOfClasses];
        for (int i = 0; i < numberOfClasses; i++) {
            classes[i].innerClassInfoIndex = reader.readu2();
            classes[i].outerClassInfoIndex = reader.readu2();
            classes[i].innerNameIndex = reader.readu2();
            classes[i].innerClassAccessFlags = reader.readu2();
        }
    }
};

#endif //JVM_INNERCLASSESATTR_H
