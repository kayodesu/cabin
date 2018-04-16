/*
 * Author: Jia Yang
 */

#ifndef JVM_JFIELD_H
#define JVM_JFIELD_H


#include "Jmember.h"

/*
    Interpretation of field descriptors
    ---------------------------------------------------
    FieldType term | Type        | Interpretation
    ---------------------------------------------------
    B              | byte signed | byte
    C              | char        | Unicode character code point in the Basic Multilingual Plane, encoded with UTF-16
    D              | double      | double-precision floating-point value
    F              | float       | single-precision floating-point value
    I              | int         | integer
    J              | long        | long integer
    L ClassName;   | reference   | an instance of class ClassName
    S              | short       | signed short
    Z              | boolean     | true or false
    [              | reference   | one array dimension
    ---------------------------------------------------
 */
class Jfield: public Jmember {
    std::string typeName;
    JclassObj *type;
public:
    int id;
    //void *extra;

    int constantValueIndex;
    const static int INVALID_CONSTANT_VALUE_INDEX = -1;

    Jfield() = default;

    Jfield(Jclass *jclass, const MemberInfo &memberInfo);

    JclassObj* getType();

    std::string toString() {
        return jclass->className + "::" + name + "~" + descriptor;
    }
};

#endif //JVM_JFIELD_H
