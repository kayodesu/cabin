#ifndef CABIN_CONSTANT_POOL_H
#define CABIN_CONSTANT_POOL_H

#include <cassert>
#include <mutex>
#include "../cabin.h"
#include "../classfile/constants.h"
#include "../slot.h"

class Class;
class Method;
class Field;
class Object;

// 从 1 开始计数，第0位无效
class ConstantPool {
    u1 *type = nullptr;
    slot_t *info = nullptr;
    u2 size = 0;

    Class *clazz = nullptr;
    mutable std::recursive_mutex mutex;

    ConstantPool() = default;

    explicit ConstantPool(Class *clazz, u2 size): size(size), clazz(clazz)
    {
        assert(clazz != nullptr);
        assert(size > 0);

        type = new u1[size];
        type[0] = JVM_CONSTANT_Invalid; // constant pool 从 1 开始计数，第0位无效

        info = new slot_t[size];
    }

public:
    ~ConstantPool()
    {
        delete[] type;
        delete[] info;
    }

    u2 getSize() const
    {
        return size;
    }

    u1 getType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        return type[i];
    }

    void setType(u2 i, u1 new_type)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        type[i] = new_type;
    }

    void setInfo(u2 i, slot_t new_info)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        info[i] = new_info;
    }

    utf8_t *utf8(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Utf8);
        return (utf8_t *)(info[i]);
    }

    utf8_t *string(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_String);
        return utf8((u2)info[i]);
    }

    utf8_t *className(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Class);
        return utf8((u2)info[i]);
    }

    utf8_t *moduleName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Module);
        return utf8((u2)info[i]);
    }

    utf8_t *packageName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Package);
        return utf8((u2)info[i]);
    }

    utf8_t *nameOfNameAndType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_NameAndType);
        return utf8((u2)info[i]);
    }

    utf8_t *typeOfNameAndType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_NameAndType);
        return utf8((u2) (info[i] >> 16));
    }

    u2 fieldClassIndex(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Fieldref);
        return (u2)info[i];
    }

    utf8_t *fieldClassName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Fieldref);
        return className((u2)info[i]);
    }

    utf8_t *fieldName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Fieldref);
        return nameOfNameAndType((u2) (info[i] >> 16));
    }

    utf8_t *fieldType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Fieldref);
        return typeOfNameAndType((u2) (info[i] >> 16));
    }

    u2 methodClassIndex(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Methodref);
        return (u2)info[i];
    }

    utf8_t *methodClassName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Methodref);
        return className((u2)info[i]);
    }

    utf8_t *methodName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Methodref);
        return nameOfNameAndType((u2) (info[i] >> 16));
    }

    utf8_t *methodType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Methodref);
        return typeOfNameAndType((u2) (info[i] >> 16));
    }

    u2 interfaceMethodClassIndex(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InterfaceMethodref);
        return (u2)info[i];
    }

    utf8_t *interfaceMethodClassName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InterfaceMethodref);
        return className((u2)info[i]);
    }

    utf8_t *interfaceMethodName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InterfaceMethodref);
        return nameOfNameAndType((u2) (info[i] >> 16));
    }

    utf8_t *interfaceMethodType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InterfaceMethodref);
        return typeOfNameAndType((u2) (info[i] >> 16));
    }

    utf8_t *methodTypeDescriptor(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_MethodType);
        return utf8((u2)info[i]);
    }

    u2 methodHandleReferenceKind(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_MethodHandle);
        return (u2) info[i];
    }

    u2 methodHandleReferenceIndex(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_MethodHandle);
        return (u2) (info[i] >> 16);
    }

    u2 invokeDynamicBootstrapMethodIndex(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InvokeDynamic);
        return (u2) info[i];
    }

    utf8_t *invokeDynamicMethodName(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InvokeDynamic);
        return nameOfNameAndType((u2) (info[i] >> 16));
    }

    utf8_t *invokeDynamicMethodType(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_InvokeDynamic);
        return typeOfNameAndType((u2) (info[i] >> 16));
    }

    jint getInt(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Integer);
        return slot::getInt(info + i);
    }

    void setInt(u2 i, jint new_int)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Integer);
        slot::setInt(info + i, new_int);
    }

    jfloat getFloat(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Float);
        return slot::getFloat(info + i);
    }

    void setFloat(u2 i, jfloat new_float)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Float);
        slot::setFloat(info + i, new_float);
    }

    jlong getLong(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Long);
        return slot::getLong(info + i);
    }

    void setLong(u2 i, jlong new_long)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Long);
        slot::setLong(info + i, new_long);
    }

    jdouble getDouble(u2 i)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Double);
        return slot::getDouble(info + i);
    }

    void setDouble(u2 i, jdouble new_double)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        assert(0 < i && i < size);
        assert(type[i] == JVM_CONSTANT_Double);
        slot::setDouble(info + i, new_double);
    }

    Class  *resolveClass(u2 i);
    Method *resolveMethod(u2 i);
    Method *resolveInterfaceMethod(u2 i);
    Method *resolveMethodOrInterfaceMethod(u2 i);
    Field  *resolveField(u2 i);
    Object *resolveString(u2 i);
    Object *resolveMethodType(u2 i);
    Object *resolveMethodHandle(u2 i);

    friend class Class;
};


#endif //CABIN_CONSTANT_POOL_H
