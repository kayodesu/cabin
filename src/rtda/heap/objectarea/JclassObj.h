/*
 * Author: Jia Yang
 */

#ifndef JVM_JCLASSOBJ_H
#define JVM_JCLASSOBJ_H


#include "Jobject.h"

class JclassObj: public Jobject {
    std::string className;
public:
    /*
     * @jclassClass: class of java/lang/Class
     */
    JclassObj(Jclass *jclassClass, const std::string &className0): Jobject(jclassClass), className(className0) {}

    const std::string& getClassName() const {
        return className;
    }
};


#endif //JVM_JCLASSOBJ_H
