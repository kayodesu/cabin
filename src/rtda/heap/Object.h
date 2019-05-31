/*
 * Author: Jia Yang
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include "../../slot.h"
#include "../ma/Class.h"


class Object {
protected:
    explicit Object(Class *c): clazz(c)
    {
        data = reinterpret_cast<slot_t *>(this + 1);

        // todo java的静态变量和类变量是有初始默认值的
//        memset(data, 0, clazz->instance_fields_count * sizeof(slot_t));
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

    void setInstFieldValue(Field *f, const slot_t *value);
    void setInstFieldValue(const char *name, const char *descriptor, const slot_t *value);

    const slot_t *getInstFieldValue(int id) const;
    const slot_t *getInstFieldValue(Field *f) const;
    const slot_t *getInstFieldValue(const char *name, const char *descriptor) const;

    bool isInstanceOf(const Class *c) const;

    // 只适用于 primitive object
    const slot_t *unbox() const;

    virtual std::string toString() const;

    void *extra;
};

#endif //JVM_JOBJECT_H
