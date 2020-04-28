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
#include "../jvmstd.h"
#include "../slot.h"
#include "field.h"

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

private:
    Field *lookupField(const char *name, const char *descriptor);

public:
    // 保存所有实例变量的值
    // 包括此Object中定义的和继承来的。
    // 特殊的，对于数组对象，保存数组的值
    slot_t *data;
    SlotsMgr slots_mgr;

    Class *clazz;

    static Object *newObject(Class *c);

    virtual size_t size() const;

    virtual bool isArrayObject() const;
    Object *clone() const; // todo ClassObject 是否支持clone??????

    void setByteField(Field *f, jbyte v);
    void setBoolField(Field *f, jbool v);
    void setCharField(Field *f, jchar v);
    void setShortField(Field *f, jshort v);
    void setIntField(Field *f, jint v);
    void setFloatField(Field *f, jfloat v);
    void setLongField(Field *f, jlong v);
    void setDoubleField(Field *f, jdouble v);
    void setRefField(Field *f, jref v);


    void setByteField(const char *name, const char *descriptor, jbyte v)
    {
        setByteField(lookupField(name, descriptor), v);
    }

    void setBoolField(const char *name, const char *descriptor, jbool v)
    {
        setBoolField(lookupField(name, descriptor), v);
    }

    void setCharField(const char *name, const char *descriptor, jchar v)
    {
        setCharField(lookupField(name, descriptor), v);
    }

    void setShortField(const char *name, const char *descriptor, jshort v)
    {
        setShortField(lookupField(name, descriptor), v);
    }

    void setIntField(const char *name, const char *descriptor, jint v)
    {
        setIntField(lookupField(name, descriptor), v);
    }

    void setFloatField(const char *name, const char *descriptor, jfloat v)
    {
        setFloatField(lookupField(name, descriptor), v);
    }

    void setLongField(const char *name, const char *descriptor, jlong v)
    {
        setLongField(lookupField(name, descriptor), v);
    }

    void setDoubleField(const char *name, const char *descriptor, jdouble v)
    {
        setDoubleField(lookupField(name, descriptor), v);
    }

    void setRefField(const char *name, const char *descriptor, jref v)
    {
        setRefField(lookupField(name, descriptor), v);
    }

//    void setFieldValue(Field *f, slot_t v); // only for category one field
    void setFieldValue(Field *f, const slot_t *value);
//    void setFieldValue(const char *name, const char *type, slot_t v); // only for category one field
//    void setFieldValue(const char *name, const char *type, const slot_t *value);
    void setFieldValue(int id, jref value);

    jbyte getByteField(Field *f);
    jbool getBoolField(Field *f);
    jchar getCharField(Field *f);
    jshort getShortField(Field *f);
    jint getIntField(Field *f);
    jfloat getFloatField(Field *f);
    jlong getLongField(Field *f);
    jdouble getDoubleField(Field *f);
    template <typename T = Object> T *getRefField(const Field *f)
    {
        assert(f != nullptr);
        return slots_mgr.getRef<T>(f->id);
    }

    jbyte getByteField(const char *name, const char *descriptor)
    {
        return getByteField(lookupField(name, descriptor));
    }

    jbool getBoolField(const char *name, const char *descriptor)
    {
        return getBoolField(lookupField(name, descriptor));
    }

    jchar getCharField(const char *name, const char *descriptor)
    {
        return getCharField(lookupField(name, descriptor));
    }

    jshort getShortField(const char *name, const char *descriptor)
    {
        return getShortField(lookupField(name, descriptor));
    }

    jint getIntField(const char *name, const char *descriptor)
    {
        return getIntField(lookupField(name, descriptor));
    }

    jfloat getFloatField(const char *name, const char *descriptor)
    {
        return getFloatField(lookupField(name, descriptor));
    }

    jlong getLongField(const char *name, const char *descriptor)
    {
        return getLongField(lookupField(name, descriptor));
    }

    jdouble getDoubleField(const char *name, const char *descriptor)
    {
        return getDoubleField(lookupField(name, descriptor));
    }

    template <typename T = Object> T *getRefField(const char *name, const char *descriptor)
    {
        return getRefField<T>(lookupField(name, descriptor));
    }

public:
    bool isInstanceOf(Class *c) const;

    const slot_t *unbox() const; // present only if primitive Object
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
