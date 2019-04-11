/*
 * Author: Jia Yang
 */

#ifndef JVM_DESCRIPTOR_H
#define JVM_DESCRIPTOR_H

/*
Field Descriptors:
A field descriptor represents the type of a class, instance, or local variable.

FieldDescriptor:
    FieldType

FieldType:
    BaseType
    ObjectType
    ArrayType

BaseType:
    (one of)
    B C D F I J S Z

ObjectType:
    LClassName;

ArrayType:
    [ComponentType

ComponentType:
    FieldType
--------------------
Method Descriptors:
A method descriptor contains zero or more parameter descriptors, representing the
types of parameters that the method takes, and a return descriptor, representing the
type of the value (if any) that the method returns.

MethodDescriptor:
    (ParameterDescriptor, ...) ReturnDescriptor

ParameterDescriptor:
    FieldType

ReturnDescriptor:
    FieldType
    VoidDescriptor

VoidDescriptor:
    V
 */

#include "../../jvmdef.h"

#define DESCRIPTOR_MAX_LEN 1023 // todo

/*
 * 解析单个 type to descriptor。
 * todo 说明
 * @len，即是输入参数又是输出参数。
 */
char* type_to_descriptor(const Object *type, char *descriptor, int *len);
Object *descriptor_to_type(ClassLoader *loader, const char *descriptor);

/*
 * @types is a array object of java/lang/Class
 * todo 说明
 */
const char* types_to_descriptor(const Object *types);

// [xxx -> [xxx
// Lx/y/z; -> x/y/z
// I -> int, ...
const char* descriptor_to_class_name(const char *descriptor, char *buf);

Object* method_descriptor_to_parameter_types(ClassLoader *loader, const char *method_descriptor);
Object* method_descriptor_to_return_type(ClassLoader *loader, const char *method_descriptor);

#endif //JVM_DESCRIPTOR_H
