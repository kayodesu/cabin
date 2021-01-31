#ifndef CABIN_DESCRIPTOR_H
#define CABIN_DESCRIPTOR_H

#include "../cabin.h"

class Array;

// @b: include
// @e：exclude
// eg. I[BLjava/lang/String;ZZ, return 5.
int numElementsInDescriptor(const char *b, const char *e);

int numElementsInMethodDescriptor(const char *method_descriptor);

std::pair<Array * /*ptypes*/, ClsObj * /*rtype*/>
parseMethodDescriptor(const char *desc, jref loader);

std::string unparseMethodDescriptor(Array *ptypes /* ClassObject *[] */, ClsObj *rtype);

// @method_type: Object of java.lang.invoke.MethodType
std::string unparseMethodDescriptor(jref method_type);

#endif //CABINDESCRIPTOR_H
