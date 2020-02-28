/*
 * Author: kayo
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include <string>
#include <unordered_map>
#include "pthread.h"
#include "slot.h"
#include "Field.h"
#include "../util/encoding.h"

class Class;

class Object {
public:
    // 对象头，放在Object类的最开始处
    union {
        struct {
            unsigned int marked: 2;
        };
        uintptr_t allFlags; // 以指针的大小对齐 todo 这样对齐有什么用
    };

private:
    pthread_mutex_t mutex; // 同一线程可重入的锁
public:
    void lock();
    void unlock();

    union {
        // present only if object of java/lang/ClassLoader
        // save the all loaded classes by this ClassLoader
        std::unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> *classes = nullptr;
        //const unicode_t *str;    // present only if object of java/lang/String
    };

protected:
    explicit Object(Class *c);

public:
    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t *data;
    Class *clazz;

    static Object *newObject(Class *c);
    static Object *newString(const utf8_t *str);

    virtual size_t size() const;

    virtual bool isArrayObject() const;
    Object *clone() const;

    void setFieldValue(Field *f, slot_t v); // only for category one field
    void setFieldValue(Field *f, const slot_t *value);
    void setFieldValue(const char *name, const char *descriptor, slot_t v); // only for category one field
    void setFieldValue(const char *name, const char *descriptor, const slot_t *value);
    void setFieldValue(int id, jref value);

private:
    const slot_t *getInstFieldValue0(const char *name, const char *descriptor) const;
public:
    template <typename T>
    T getInstFieldValue(const char *name, const char *descriptor) const
    {
        return * (T *) getInstFieldValue0(name, descriptor);
    }

    template <typename T>
    T getInstFieldValue(const Field *f) const
    {
        assert(f != nullptr);
        return * (T *) (data + f->id);
    }

    bool isInstanceOf(Class *c) const;

    const slot_t *unbox() const; // present only if primitive Object
    utf8_t *toUtf8() const;       // present only if String Object

    virtual std::string toString() const;
};

static inline Object *newObject(Class *c)
{
    return Object::newObject(c);
}

static inline jstrref newString(const utf8_t *str)
{
    return Object::newString(str);
}

struct StrObjEquals {
    bool operator()(Object *x, Object *y) const;
};

struct StrObjHash {
    size_t operator()(Object *x) const;
};

#endif //JVM_JOBJECT_H
