/*
 * Author: Jia Yang
 */

#ifndef JVM_PRIMITIVETYPE_H
#define JVM_PRIMITIVETYPE_H

#include <string>

struct PrimitiveType {
    std::string name;
	std::string descriptor;
    std::string arrayClassName;
    std::string wrapperClassName;
};

static PrimitiveType primitiveTypes[] = {
    PrimitiveType{ "void",    "V", "[V", "java/lang/Void" },
    PrimitiveType{ "boolean", "Z", "[Z", "java/lang/Boolean" },
    PrimitiveType{ "byte",    "B", "[B", "java/lang/Byte" },
    PrimitiveType{ "char",    "C", "[C", "java/lang/Character" },
    PrimitiveType{ "short",   "S", "[S", "java/lang/Short" },
    PrimitiveType{ "int",     "I", "[I", "java/lang/Integer" },
    PrimitiveType{ "long",    "J", "[J", "java/lang/Long" },
    PrimitiveType{ "float",   "F", "[F", "java/lang/Float" },
    PrimitiveType{ "double",  "D", "[D", "java/lang/Double" },
};

static bool isPrimitiveByClassName(const std::string &className) {
    for (auto &primitiveType : primitiveTypes) {
        if (primitiveType.wrapperClassName == className) {
            return true;
        }
    }
    return false;
}

#endif //JVM_PRIMITIVETYPE_H
