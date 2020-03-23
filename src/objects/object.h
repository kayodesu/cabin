/*
 * Author: kayo
 */

#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

//#include <mutex>
#include <string>
#include <unordered_map>
#include "../classfile/constants.h"
#include "../util/encoding.h"
#include "../kayo.h"
#include "../native/jni.h"

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

    JNIObjRefType jni_obj_ref_type = JNIInvalidRefType;

    static Object *newObject(Class *c);

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
//    void setFieldValue(const char *name, const char *type, slot_t v); // only for category one field
//    void setFieldValue(const char *name, const char *type, const slot_t *value);
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

static inline Object *newObject(Class *c)
{
    return Object::newObject(c);
}


#endif //JVM_JOBJECT_H
