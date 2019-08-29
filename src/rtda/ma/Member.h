/*
 * Author: kayo
 */

#ifndef JVM_MEMBER_H
#define JVM_MEMBER_H

#include <string>
#include "Access.h"

class Class;
class ClassObject;
class ClassLoader;

/*
 * 类的成员，包括 Field and Method
 */
struct Member: public Access {
    /*
     * which class this Member belongs to.
     *
     * 对于方法class 表示定义此方法的类。
     * 对于一个子类，如果有一个方法是从父类中继承而来的，
     * 那么此方法的 class 依然指向父类，即真正定义此方法的类。
     */
    Class *clazz;
    const char *name = nullptr;
    const char *descriptor = nullptr;

    bool deprecated = false;
    const char *signature = nullptr;

    bool isAccessibleTo(const Class *visitor) const;

protected:
    explicit Member(Class *c): clazz(c) { }
};

// [xxx -> [xxx
// Lx/y/z; -> x/y/z
// I -> int, ...
const std::string descriptorToClassName(const char *descriptor);

//ClassObject *descriptorToType(ClassLoader *loader, const char *descriptor);

#endif //JVM_MEMBER_H
