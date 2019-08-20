/*
 * Author: kayo
 */

#ifndef JVM_CLSOBJ_H
#define JVM_CLSOBJ_H

#include <string>
#include "Object.h"

class Class;

// Object of java/lang/Class
class ClassObject: public Object {
    /*
     * @entityClass: todo 说明
     */
    explicit ClassObject(Class *classClass, Class *entityClass): Object(classClass), entityClass(entityClass)
    {
        data = reinterpret_cast<slot_t *>(this + 1); // todo data怎么办,这样不对
    }
public:
    // save the entity class (class, interface, array class, primitive type, or void) represented by this Object
    Class *entityClass;

    static void operator delete(void *rawMemory,std::size_t size) throw();

    static ClassObject *newInst(Class *entityClass);

//    virtual size_t size() const;
    std::string toString() const override;
};

#endif //JVM_CLSOBJ_H
