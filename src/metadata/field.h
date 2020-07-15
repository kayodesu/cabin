/*
 * Author: Yo Ka
 */

#ifndef KAYO_FIELD_H
#define KAYO_FIELD_H

#include <vector>
#include "../classfile/attributes.h"
#include "../classfile/constants.h"

class Class;
class ClassObject;

class Field {
public:    
    Class *clazz = nullptr; // 定义此 Field 的类
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    int accsee_flags;

    bool deprecated = false;
    const char *signature = nullptr;

private:
    // the declared type(class Object) of this field
    // like, int k; the type of k is int.class
    ClassObject *type = nullptr;
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
            slot_t data[2];
        } static_value = {};

        int id;
    };

    std::vector<Annotation> rt_visi_annos;   // runtime visible annotations
    std::vector<Annotation> rt_invisi_annos; // runtime invisible annotations

public:
    Field(Class *c, BytecodeReader &r);

    ClassObject *getType();

    bool isPrim() const;

    std::string toString() const;
    friend std::ostream &operator <<(std::ostream &os, const Field &field);

    bool isPublic() const    { return accIsPublic(accsee_flags); }
    bool isProtected() const { return accIsProtected(accsee_flags); }
    bool isPrivate() const   { return accIsPrivate(accsee_flags); }
    bool isStatic() const    { return accIsStatic(accsee_flags); }
    bool isFinal() const     { return accIsFinal(accsee_flags); }
    bool isTransient() const { return accIsTransient(accsee_flags); }
    bool isVolatile() const  { return accIsVolatile(accsee_flags); }

    void setSynthetic() { accSetSynthetic(accsee_flags); }
};


#endif //KAYO_FIELD_H
