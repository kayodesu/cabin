/*
 * Author: Jia Yang
 */

#ifndef JVM_ANNOTATIONDEFAULTATTR_H
#define JVM_ANNOTATIONDEFAULTATTR_H

#include "Attribute.h"
#include "ElementValue.h"

/*
 * The AnnotationDefault attribute is a variable-length attribute in the attributes
 * table of certain method_info structures , namely those representing elements
 * of annotation types . The AnnotationDefault attribute records the
 * default value for the element represented by the method_info
 * structure. The Java Virtual Machine must make this default value available so it
 * can be applied by appropriate reflective APIs.

 * There may be at most one AnnotationDefault attribute in the attributes table
 * of a method_info structure which represents an element of an annotation type.
 */
struct AnnotationDefaultAttr: public Attribute {
    ElementValue *defaultValue;

    explicit AnnotationDefaultAttr(BytecodeReader &reader): Attribute(reader) {
        defaultValue = new ElementValue(reader);
    }
};

#endif //JVM_ANNOTATIONDEFAULTATTR_H
