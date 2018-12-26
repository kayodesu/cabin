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

#define DESCRIPTOR_MAX_LEN 1023 // todo

struct object;
struct classloader;

/*
 * 解析单个 type to descriptor。
 * todo 说明
 * @len，即是输入参数又是输出参数。
 */
char* type_to_descriptor(const struct object *type, char *descriptor, int *len);
struct object *descriptor_to_type(struct classloader *loader, const char *descriptor);

/*
 * @types is a array object of java/lang/Class
 * todo 说明
 */
const char* types_to_descriptor(const struct object *types);

// [xxx -> [xxx
// Lx/y/z; -> x/y/z
// I -> int, ...
const char* descriptor_to_class_name(const char *descriptor, char *buf);

struct object* method_descriptor_to_parameter_types(struct classloader *loader, const char *method_descriptor);
struct object* method_descriptor_to_return_type(struct classloader *loader, const char *method_descriptor);

#endif //JVM_DESCRIPTOR_H
