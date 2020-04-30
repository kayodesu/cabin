/*
 * Author: Yo Ka
 */

#ifndef KAYO_FIELD_H
#define KAYO_FIELD_H

#include <vector>
#include "Modifier.h"
#include "../classfile/attributes.h"

class Class;

class Field {
public:
    // 定义此 Field 的类
    Class *clazz;
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    jint modifiers;

    bool deprecated = false;
    const char *signature = nullptr;

private:
    // the declared type(class Object) of this field
    // like, int k; the type of k is int.class
    Class *type = nullptr;
public:
    bool category_two;

    union {
        // static value 必须初始化清零
        union {
            jbool z;
            jbyte b;
            jchar c;
            jshort s;
            jint i;
            jlong j;
            jfloat f;
            jdouble d;
            jref r;
            slot_t data[2] = { 0, 0 };
        } staticValue;

        int id = 0;
    };

    std::vector<Annotation> rtVisiAnnos;   // runtime visible annotations
    std::vector<Annotation> rtInvisiAnnos; // runtime invisible annotations

public:
    Field(Class *c, BytecodeReader &r);

    Class *getType();

    bool isPrim() const;

    std::string toString() const;
    friend std::ostream &operator <<(std::ostream &os, const Field &field);

    bool isPublic() const    { return Modifier::isPublic(modifiers); }
    bool isProtected() const { return Modifier::isProtected(modifiers); }
    bool isPrivate() const   { return Modifier::isPrivate(modifiers); }
    bool isStatic() const    { return Modifier::isStatic(modifiers); }
    bool isFinal() const     { return Modifier::isFinal(modifiers); }
    bool isTransient() const { return Modifier::isTransient(modifiers); }
    bool isVolatile() const  { return Modifier::isVolatile(modifiers); }

    void setSynthetic() { Modifier::setSynthetic(modifiers); }
};


#endif //KAYO_FIELD_H
