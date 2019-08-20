/*
 * Author: kayo
 */

#ifndef JVM_PRIMITIVE_TYPES_H
#define JVM_PRIMITIVE_TYPES_H


void loadPrimitiveTypes();

bool isPrimitiveClassName(const char *class_name);
bool isPrimitiveDescriptor(char descriptor);

const char *primitiveClassName2arrayClassName(const char *class_name);
const char *primitiveDescriptor2className(char descriptor);
char primitiveClassName2descriptor(const char *class_name);

#endif //JVM_PRIMITIVE_TYPES_H
