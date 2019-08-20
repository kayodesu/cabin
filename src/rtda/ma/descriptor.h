/*
 * Author: kayo
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
A Method descriptor contains zero or more parameter descriptors, representing the
types of parameters that the Method takes, and a return descriptor, representing the
type of the value (if any) that the Method returns.

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

class Object;
class ArrayObject;
class ClassObject;
class ClassLoader;


/*
 * @types is a array Object of java/lang/Class or just Object of java/lang/Class
 * todo 说明
 */
std::string typesToDescriptor(const Object *types);


ClassObject *descriptorToType(ClassLoader *loader, const char *descriptor);
// [xxx -> [xxx
// Lx/y/z; -> x/y/z
// I -> int, ...
const char *descriptorToClassName(const char *descriptor, char *buf);

ArrayObject *methodDescriptorToParameterTypes(ClassLoader *loader, const char *method_descriptor);
ClassObject *methodDescriptorToTeturnType(ClassLoader *loader, const char *method_descriptor);

#endif //JVM_DESCRIPTOR_H
