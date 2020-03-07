/*
 * Author: kayo
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

//#include <mutex>
#include <string>
#include <unordered_map>
#include "../util/encoding.h"

class Field;
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

//private:
//    std::recursive_mutex mutex;
//public:
//    void lock() { mutex.lock(); }
//    void unlock() { mutex.unlock(); }

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
    Object *clone() const; // todo ClassObject 是否支持clone??????

    void setFieldValue(Field *f, jint v);
    void setFieldValue(Field *f, jfloat v);
    void setFieldValue(Field *f, jlong v);
    void setFieldValue(Field *f, jdouble v);
    void setFieldValue(Field *f, jref v);

    void setFieldValue(const char *name, const char *descriptor, jint v);
    void setFieldValue(const char *name, const char *descriptor, jfloat v);
    void setFieldValue(const char *name, const char *descriptor, jlong v);
    void setFieldValue(const char *name, const char *descriptor, jdouble v);
    void setFieldValue(const char *name, const char *descriptor, jref v);

//    void setFieldValue(Field *f, slot_t v); // only for category one field
    void setFieldValue(Field *f, const slot_t *value);
//    void setFieldValue(const char *name, const char *descriptor, slot_t v); // only for category one field
//    void setFieldValue(const char *name, const char *descriptor, const slot_t *value);
    void setFieldValue(int id, jref value);

private:
    const slot_t *getInstFieldValue0(const Field *f) const;
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
        return * (T *) getInstFieldValue0(f);
    }

    bool isInstanceOf(Class *c) const;

    const slot_t *unbox() const; // present only if primitive Object
    utf8_t *toUtf8() const;       // present only if Object of java/lang/String
    // present only if object of java/lang/ClassLoader
    // save the all loaded classes by this ClassLoader
    std::unordered_map<const utf8_t *, Class *, utf8::Hash, utf8::Comparator> *classes = nullptr;

    virtual std::string toString() const;
};


// Object of array
class Array: public Object {
    Array(Class *ac, jint arrLen);
    Array(Class *ac, jint dim, const jint lens[]);

public:
    jsize len; // 数组的长度

    static Array *newArray(Class *ac, jint arrLen);
    static Array *newMultiArray(Class *ac, jint dim, const jint lens[]);

    bool isArrayObject() const override { return true; }
    bool isPrimArray() const;

    bool checkBounds(jint index)
    {
        return 0 <= index && index < len;
    }

    void *index(jint index0) const;

    template <typename T>
    void set(jint index0, T data)
    {
        *(T *) index(index0) = data;
    }

    void set(int index0, jref value);

    template <typename T>
    T get(jint index0) const
    {
        return *(T *) index(index0);
    }


    static void copy(Array *dst, jint dst_pos, const Array *src, jint src_pos, jint len);
    size_t size() const override;
    //Array *clone() const;
    std::string toString() const override;
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

static inline Array *newArray(Class *ac, jint arrLen)
{
    return Array::newArray(ac, arrLen);
}

static inline Array *newMultiArray(Class *ac, jint dim, const jint lens[])
{
    return Array::newMultiArray(ac, dim, lens);
}

#endif //JVM_JOBJECT_H
