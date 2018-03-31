/*
 * Author: Jia Yang
 */

#ifndef JVM_RUNTIMEANNOTATIONSATTR_H
#define JVM_RUNTIMEANNOTATIONSATTR_H


#include "Attribute.h"
#include "Annotation.h"

/*
 * The RuntimeVisibleAnnotations attribute records run-time visible
 * annotations on the declaration of the corresponding class, field, or method.
 * The Java Virtual Machine must make these annotations available so they can be
 * returned by the appropriate reflective APIs.
 *
 * The RuntimeInvisibleAnnotations attribute records run-time
 * invisible annotations on the declaration of the corresponding class, method, or field.
 * There may be at most one RuntimeInvisibleAnnotations attribute in the
 * attributes table of a ClassFile, field_info, or method_info structure.

 * The RuntimeInvisibleAnnotations attribute is similar to the
 * RuntimeVisibleAnnotations attribute (§4.7.16), except that the annotations
 * represented by a RuntimeInvisibleAnnotations attribute must not be made available
 * for return by reflective APIs, unless the Java Virtual Machine has been instructed to retain
 * these annotations via some implementation-specific mechanism such as a command line
 * flag. In the absence of such instructions, the Java Virtual Machine ignores this attribute.
 */
struct RuntimeAnnotationsAttr: Attribute {
    u2 numAnnotations;
    Annotation *annotations; // [num_annotations];

    explicit RuntimeAnnotationsAttr(BytecodeReader &reader): Attribute(reader) {
        numAnnotations = reader.readu2();
        annotations = new Annotation[numAnnotations];
        for (int i = 0; i < numAnnotations; i++) {
            new (annotations + i)Annotation(reader);//parse_annotation(reader, constant_pool);
        }
    }
};// runtime_annotations_attr;//, runtime_visible_annotations_attr, runtime_invisible_annotations_attr;
typedef RuntimeAnnotationsAttr RuntimeVisibleAnnotationsAttr;
typedef RuntimeAnnotationsAttr RuntimeInvisibleAnnotationsAttr;


#endif //JVM_RUNTIMEANNOTATIONSATTR_H
