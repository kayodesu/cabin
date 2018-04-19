/*
 * Author: Jia Yang
 */

#ifndef JVM_IOEXCEPTION_H
#define JVM_IOEXCEPTION_H


#include <exception>
#include <string>

class IoException: public std::exception {
    std::string msg;
public:
    explicit IoException(const char *__msg) {
        msg = __msg;
    }

    explicit IoException(const std::string &__msg) {
        msg = __msg;
    }

    const char* what() const throw() {
        return msg.c_str();
    }
};


#endif //JVM_IOEXCEPTION_H
