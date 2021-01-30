#ifndef JVM_JOBJECT_H
#define JVM_JOBJECT_H

#include <cassert>
#include <mutex>
#include <string>
#include <unordered_map>
#include "../classfile/constants.h"
#include "../util/encoding.h"
#include "../jvmstd.h"
#include "../slot.h"
#include "../metadata/field.h"

class Field;
class Class;

class Object {
public:
    // 对象头，放在Object类的最开始处
    union {
        struct {
            unsigned int accessible: 1; // gc时判断对象是否可达
            unsigned int marked: 2;
        };
        uintptr_t all_flags; // 以指针的大小对齐 todo 这样对齐有什么用
    };

private:
    std::recursive_mutex mutex;
public:
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }

protected:
    explicit Object(Class *c);

private:
    Field *lookupField(const char *name, const char *descriptor);

public:
    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t *data;

    Class *clazz;

    static Object *newObject(Class *c);

    virtual size_t size() const;

    virtual bool isArrayObject() const;
    virtual Object *clone() const;

#define setTField(T, t) \
    void set##T##Field(Field *f, t v) \
    { \
        assert(f != nullptr); \
        slot::set##T(data + f->id, v); \
    } \
    \
    void set##T##Field(const char *name, const char *descriptor, t v) \
    {\
        assert(name != nullptr && descriptor != nullptr); \
        set##T##Field(lookupField(name, descriptor), v);\
    }

    setTField(Byte, jbyte)
    setTField(Bool, jbool)
    setTField(Char, jchar)
    setTField(Short, jshort)
    setTField(Int, jint)
    setTField(Float, jfloat)
    setTField(Long, jlong)
    setTField(Double, jdouble)
    setTField(Ref, jref)
#undef setTField


//    void setFieldValue(Field *f, slot_t v); // only for category one field
    void setFieldValue(Field *f, const slot_t *value);
//    void setFieldValue(const char *name, const char *type, slot_t v); // only for category one field
//    void setFieldValue(const char *name, const char *type, const slot_t *value);
    void setFieldValue(int id, jref value);

#define getTField(T, t) \
    t get##T##Field(Field *f) \
    { \
        assert(f != nullptr); \
        return slot::get##T(data + f->id); \
    } \
    \
    t get##T##Field(const char *name, const char *descriptor) \
    {\
        assert(name != nullptr && descriptor != nullptr); \
        return get##T##Field(lookupField(name, descriptor));\
    }

    getTField(Byte, jbyte)
    getTField(Bool, jbool)
    getTField(Char, jchar)
    getTField(Short, jshort)
    getTField(Int, jint)
    getTField(Float, jfloat)
    getTField(Long, jlong)
    getTField(Double, jdouble)
#undef getTField

    template <typename T = Object> T *getRefField(const Field *f)
    {
        assert(f != nullptr);
        return (T *) slot::getRef(data + f->id);
    }

    template <typename T = Object> T *getRefField(const char *name, const char *descriptor)
    {
        assert(name != nullptr && descriptor != nullptr);
        return (T *) getRefField(lookupField(name, descriptor));
    }

public:
    bool isInstanceOf(Class *c) const;

    const slot_t *unbox() const; // present only if primitive box Object
    utf8_t *toUtf8() const;      // present only if Object of java/lang/String
    
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
