/*
 * Author: Jia Yang
 */

#ifndef JVM_CLASSNOTFINDEXCEPTION_H
#define JVM_CLASSNOTFINDEXCEPTION_H


#include <exception>
#include <string>

class ClassNotFindException: public std::exception {
    std::string msg;
public:
    explicit ClassNotFindException(const char *__msg) {
        msg = __msg;
    }

    explicit ClassNotFindException(const std::string &__msg) {
        msg = __msg;
    }

    const char* what() const throw() {
        return msg.c_str();
    }
};

#endif //JVM_CLASSNOTFINDEXCEPTION_H
