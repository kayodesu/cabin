/*
 * Author: kayo
 */

#ifndef JVM_STROBJ_H
#define JVM_STROBJ_H

#include "Object.h"
#include "../../utf8.h"

// Object of java/lang/String
class StringObject: public Object {
    const jchar *raw; // 保存字符串的值。
    const char *utf8Value = nullptr;
    jint len;
    explicit StringObject(const char *str);

public:
    const char *getUtf8Value();

//    static void *operator new(std::size_t size) throw(std::bad_alloc);
    static StringObject *newInst(const char *str);
    static void operator delete(void *rawMemory, std::size_t size) throw();

//    bool operator<(const StringObject &right) const;
//    virtual size_t size() const;
    std::string toString() const override;
};

#endif //JVM_STROBJ_H
