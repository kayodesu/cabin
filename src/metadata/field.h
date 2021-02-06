#ifndef CABIN_FIELD_H
#define CABIN_FIELD_H

#include <vector>
#include "../classfile/attributes.h"
#include "../classfile/constants.h"

class Class;

class Field {
public:    
    Class *clazz = nullptr; // 定义此 Field 的类
    const utf8_t *name = nullptr;
    const utf8_t *descriptor = nullptr;

    int access_flags;

    bool deprecated = false;
    const char *signature = nullptr;

private:
    // the declared type(class Object) of this field
    // like, int k; the type of k is int.class
    ClsObj *type = nullptr;
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

private:
    Field(Class *clazz, const utf8_t *name, const utf8_t *descriptor, int access_flags)
            : clazz(clazz), name(name), descriptor(descriptor), access_flags(access_flags)
    {
        assert(clazz != nullptr);
        assert(name != nullptr);
        assert(descriptor != nullptr);
        category_two = (descriptor[0] == 'J' || descriptor[0]== 'D');

        if (isStatic()) {
            memset(&static_value, 0, sizeof(static_value));
        } else {
            id = 0; // todo id 应该怎么设置？？
        }
    }

public:
    ClsObj *getType();

    [[nodiscard]] bool isPrim() const;

    [[nodiscard]] std::string toString() const;
    friend std::ostream &operator <<(std::ostream &os, const Field &field);

    [[nodiscard]] bool isPublic() const    { return accIsPublic(access_flags); }
    [[nodiscard]] bool isProtected() const { return accIsProtected(access_flags); }
    [[nodiscard]] bool isPrivate() const   { return accIsPrivate(access_flags); }
    [[nodiscard]] bool isStatic() const    { return accIsStatic(access_flags); }
    [[nodiscard]] bool isFinal() const     { return accIsFinal(access_flags); }
    [[nodiscard]] bool isTransient() const { return accIsTransient(access_flags); }
    [[nodiscard]] bool isVolatile() const  { return accIsVolatile(access_flags); }

    void setSynthetic() { accSetSynthetic(access_flags); }

    friend class Class;
};

#endif //CABIN_FIELD_H
