/*
 * Author: kayo
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include "../../slot.h"
#include "../ma/Class.h"
#include "../ma/Field.h"

class Object {
protected:
    explicit Object(Class *c): clazz(c)
    {
        data = reinterpret_cast<slot_t *>(this + 1);
    }

public:
    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t *data;
    Class *clazz;

    static Object *newInst(Class *c);
    static void operator delete(void *rawMemory, std::size_t size) throw();

    virtual size_t size() const;

    bool isArray() const;
    Object *clone() const;

    void setFieldValue(Field *f, slot_t v); // only for category one field
    void setFieldValue(Field *f, const slot_t *value);
    void setFieldValue(const char *name, const char *descriptor, slot_t v); // only for category one field
    void setFieldValue(const char *name, const char *descriptor, const slot_t *value);

    // @id: id of the field
    template <typename T>
    T getInstFieldValue(int id) const
    {
        assert(0 <= id && id < clazz->instFieldsCount);
        return * (T *) (data + id);
    }

    template <typename T>
    T getInstFieldValue(Field *f) const
    {
        assert(f != nullptr);
        return getInstFieldValue<T>(f->id);
    }

    template <typename T>
    T getInstFieldValue(const char *name, const char *descriptor) const
    {
        assert(name != nullptr && descriptor != nullptr);

        Field *f = clazz->lookupField(name, descriptor);
        if (f == nullptr) {
            jvm_abort("error, %s, %s\n", name, descriptor); // todo
        }

        return getInstFieldValue<T>(f->id);
    }

    void storeInstFieldValue(Field *f, slot_t *&value) const;

    bool isInstanceOf(const Class *c) const;

    // 只适用于 primitive object
    const slot_t *unbox() const;

    virtual std::string toString() const;

    void *extra;
};

#endif //JVM_JOBJECT_H
