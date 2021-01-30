#ifndef CABIN_CLASS_OBJECT_H
#define CABIN_CLASS_OBJECT_H

#include "object.h"

class Class;

/*
 * Object of java.lang.Class
 * 只能由虚拟机创建
 */
class ClassObject: public Object {
    ClassObject(Class *c);

public:    
    Class *jvm_mirror;
    
    virtual Object *clone() const;

    friend ClassObject *generateClassObject(Class *c);
};

ClassObject *generateClassObject(Class *c);

#endif // CABIN_CLASS_OBJECT_H