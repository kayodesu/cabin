/*
 * Author: Jia Yang
 */

#ifndef JVM_BASE_H
#define JVM_BASE_H

#include <string>
#include <sstream>

class Base {
public:
    virtual std::string toString() const {
        std::stringstream ss;
        ss << this;
        ss << "\n";
        return ss.str();
    }
};


#endif //JVM_BASE_H
