/*
 * Author: Jia Yang
 */

#ifndef JVM_RUNTIMEPARAMETERANNOTATIONSATTR_H
#define JVM_RUNTIMEPARAMETERANNOTATIONSATTR_H

#include "Attribute.h"
#include "Annotation.h"

/*
 * The RuntimeVisibleParameterAnnotations attribute records run-time visible
 * annotations on the declarations of formal parameters of the corresponding method.
 * The Java Virtual Machine must make these annotations available so they can be
 * returned by the appropriate reflective APIs.

 * There may be at most one RuntimeVisibleParameterAnnotations attribute in
 * the attributes table of a method_info structure.

 * The RuntimeInvisibleParameterAnnotations attribute records run-time invisible
 * annotations on the declarations of formal parameters of the corresponding method.

 * There may be at most one RuntimeInvisibleParameterAnnotations attribute in
 * the attributes table of a method_info structure.

 * The RuntimeInvisibleParameterAnnotations attribute is similar to the
 * RuntimeVisibleParameterAnnotations attribute (§4.7.18), except that the
 * annotations represented by a RuntimeInvisibleParameterAnnotations attribute
 * must not be made available for return by reflective APIs, unless the Java Virtual Machine
 * has specifically been instructed to retain these annotations via some implementation-specific
 * mechanism such as a command line flag. In the absence of such instructions, the
 * Java Virtual Machine ignores this attribute.
 */
struct RuntimeParameterAnnotationsAttr: public Attribute {
    u2 numParameters;
    struct ParameterAnnotation {
        u2 numAnnotations;
        Annotation *annotations; // [num_annotations];
        void init(BytecodeReader &reader) {
            numAnnotations = reader.readu2();
            annotations = new Annotation[numAnnotations];
            for (int i = 0; i < numAnnotations; i++) {
                new (annotations + i)Annotation(reader);
            }
        }
    };
    ParameterAnnotation *parameterAnnotations; // [num_parameters];

    RuntimeParameterAnnotationsAttr(BytecodeReader &reader): Attribute(reader) {
        numParameters = reader.readu2();
        parameterAnnotations = new ParameterAnnotation[numParameters];
        for (int i = 0; i < numParameters; i++) {
            parameterAnnotations[i].init(reader);
        }
    }
};//, runtime_visible_parameter_annotations_attr, runtime_invisible_parameter_annotations_attr;
typedef RuntimeParameterAnnotationsAttr RuntimeVisibleParameterAnnotationsAttr;
typedef RuntimeParameterAnnotationsAttr RuntimeInvisibleParameterAnnotationsAttr;

#endif //JVM_RUNTIMEPARAMETERANNOTATIONSATTR_H
