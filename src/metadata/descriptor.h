/*
 * Author: Yo Ka
 */

#ifndef YOVM_DESCRIPTOR_H
#define YOVM_DESCRIPTOR_H

#include "../jvmstd.h"

class ClassObject;
class Array;

// @b: include
// @e：exclude
// eg. I[BLjava/lang/String;ZZ, return 5.
int numElementsInDescriptor(const char *b, const char *e);

int numElementsInMethodDescriptor(const char *method_descriptor);

std::pair<Array * /*ptypes*/, ClassObject * /*rtype*/>
parseMethodDescriptor(const char *desc, jref loader);

std::string unparseMethodDescriptor(Array *ptypes /* ClassObject *[] */, ClassObject *rtype);

// @method_type: Object of java.lang.invoke.MethodType
std::string unparseMethodDescriptor(jref method_type);

#endif //YOVM_DESCRIPTOR_H
