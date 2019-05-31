/*
 * Author: Jia Yang
 */

#ifndef JVM_MEMBER_H
#define JVM_MEMBER_H

#include "Access.h"

class Class;

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
    const char *name;
    const char *descriptor;

    bool deprecated = false;
    const char *signature;

    bool isAccessibleTo(const Class *visitor) const;

protected:
    explicit Member(Class *c): clazz(c) { }
};


#endif //JVM_MEMBER_H
