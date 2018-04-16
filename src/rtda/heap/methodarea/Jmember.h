/*
 * Author: Jia Yang
 */

#ifndef JVM_JMEMBER_H
#define JVM_JMEMBER_H


#include "../../../jvm.h"
#include "../../../BytecodeReader.h"
#include "RTCP.h"
#include "AccessPermission.h"
#include "../../../interpreter/StackFrame.h"

class Jclass;

/*
 * member of class, include field and method.
 */
class Jmember: public AccessPermission {
public:
    Jclass *jclass; // which class this member belongs to

    std::string name;
    std::string descriptor;
//public:
    bool isAccessibleTo(const Jclass *visitor) const;

    Jmember(){}

    explicit Jmember(const Jmember &m) {
        jclass = m.jclass;
        name = m.name;
        descriptor = m.descriptor;
    }
};


#endif //JVM_JMEMBER_H
