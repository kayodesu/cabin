/*
 * Author: kayo
 */

#ifndef KAYO_CONSTANT_POOL_H
#define KAYO_CONSTANT_POOL_H

#include <cassert>
#include "../vmdef.h"
#include "../classfile/constants.h"
#include "../slot.h"

class Class;
class Method;
class Field;
class Object;

// 从 1 开始计数，第0位无效
class ConstantPool {
    u1 *_type = nullptr;
    slot_t *_info = nullptr;

public:
    Class *clazz;
    u2 size = 0;

    ConstantPool() = default;

    explicit ConstantPool(Class *clazz, u2 size): clazz(clazz), size(size)
    {
        assert(clazz != nullptr);
        assert(size > 0);

        _type = new u1[size];
        _type[0] = JVM_CONSTANT_Invalid; // constant pool 从 1 开始计数，第0位无效

        _info = new slot_t[size];
    }

    ~ConstantPool()
    {
        delete[] _type;
        delete[] _info;
    }

    u1 type(u2 i)
    {
        assert(0 < i && i < size);
        return _type[i];
    }

    void type(u2 i, u1 newType)
    {
        assert(0 < i && i < size);
        _type[i] = newType;
    }

    slot_t info(u2 i)
    {
        assert(0 < i && i < size);
        return _info[i];
    }

    void info(u2 i, slot_t newInfo)
    {
        assert(0 < i && i < size);
        _info[i] = newInfo;
    }

    utf8_t *utf8(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Utf8);
        return (utf8_t *)(_info[i]);
    }

    utf8_t *string(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_String);
        return utf8((u2)_info[i]);
    }

    utf8_t *className(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Class);
        return utf8((u2)_info[i]);
    }

    utf8_t *moduleName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Module);
        return utf8((u2)_info[i]);
    }

    utf8_t *packageName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Package);
        return utf8((u2)_info[i]);
    }

    utf8_t *nameOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_NameAndType);
        return utf8((u2)_info[i]);
    }

    utf8_t *typeOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_NameAndType);
        return utf8((u2) (_info[i] >> 16));
    }

    u2 fieldClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Fieldref);
        return (u2)_info[i];
    }

    utf8_t *fieldClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Fieldref);
        return className((u2)_info[i]);
    }

    utf8_t *fieldName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Fieldref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *fieldType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Fieldref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 methodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Methodref);
        return (u2)_info[i];
    }

    utf8_t *methodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Methodref);
        return className((u2)_info[i]);
    }

    utf8_t *methodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Methodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Methodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 interfaceMethodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InterfaceMethodref);
        return (u2)_info[i];
    }

    utf8_t *interfaceMethodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InterfaceMethodref);
        return className((u2)_info[i]);
    }

    utf8_t *interfaceMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InterfaceMethodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *interfaceMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InterfaceMethodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodTypeDescriptor(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_MethodType);
        return utf8((u2)_info[i]);
    }

    u2 methodHandleReferenceKind(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_MethodHandle);
        return (u2) _info[i];
    }

    u2 methodHandleReferenceIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_MethodHandle);
        return (u2) (_info[i] >> 16);
    }

    u2 invokeDynamicBootstrapMethodIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InvokeDynamic);
        return (u2) _info[i];
    }

    utf8_t *invokeDynamicMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InvokeDynamic);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *invokeDynamicMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_InvokeDynamic);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    jint _int(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Integer);
        return ISLOT(_info + i);
    }

    void _int(u2 i, jint newInt)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Integer);
        ISLOT(_info + i) = newInt;
    }

    jfloat _float(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Float);
        return FSLOT(_info + i);
    }

    void _float(u2 i, jfloat newFloat)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Float);
        FSLOT(_info + i) = newFloat;
    }

    jlong _long(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Long);
        return LSLOT(_info + i);
    }

    void _long(u2 i, jlong newLong)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Long);
        LSLOT(_info + i) = newLong;
    }

    jdouble _double(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Double);
        return DSLOT(_info + i);
    }

    void _double(u2 i, jdouble newDouble)
    {
        assert(0 < i && i < size);
        assert(_type[i] == JVM_CONSTANT_Double);
        DSLOT(_info + i) = newDouble;
    }

    Class *resolveClass(u2 i);
    Method *resolveMethod(u2 i);
    Method *resolveInterfaceMethod(u2 i);
    Field *resolveField(u2 i);
    Object *resolveString(u2 i);
    Object *resolveMethodType(u2 i);
    Object *resolveMethodHandle(u2 i);
};


#endif //KAYO_CONSTANT_POOL_H
