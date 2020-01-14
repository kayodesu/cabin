/*
 * Author: kayo
 */

#ifndef KAYOVM_CONSTANTPOOL_H
#define KAYOVM_CONSTANTPOOL_H

#include "slot.h"
#include "../util/BytecodeReader.h"
#include "../classfile/constant.h"

class Class;
class Method;
class Field;

// 从 1 开始计数，第0位无效
struct ConstantPool {
    Class *clazz;
    u2 size = 0;
    u1 *_type = nullptr;
    slot_t *_info = nullptr;

    ConstantPool() = default;

    explicit ConstantPool(Class *clazz, u2 size): clazz(clazz), size(size)
    {
        assert(clazz != nullptr);
        assert(size > 0);

        _type = new u1[size];
        _type[0] = CONSTANT_Invalid; // constant pool 从 1 开始计数，第0位无效

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
        assert(_type[i] == CONSTANT_Utf8);
        return (utf8_t *)(_info[i]);
    }

    utf8_t *string(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_String);
        return utf8((u2)_info[i]);
    }

    utf8_t *className(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Class);
        return utf8((u2)_info[i]);
    }

    utf8_t *nameOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_NameAndType);
        return utf8((u2)_info[i]);
    }

    utf8_t *typeOfNameAndType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_NameAndType);
        return utf8((u2) (_info[i] >> 16));
    }

    u2 fieldClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return (u2)_info[i];
    }

    utf8_t *fieldClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return className((u2)_info[i]);
    }

    utf8_t *fieldName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *fieldType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Fieldref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 methodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return (u2)_info[i];
    }

    utf8_t *methodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return className((u2)_info[i]);
    }

    utf8_t *methodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Methodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    u2 interfaceMethodClassIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return (u2)_info[i];
    }

    utf8_t *interfaceMethodClassName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return className((u2)_info[i]);
    }

    utf8_t *interfaceMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *interfaceMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InterfaceMethodref);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *methodTypeDescriptor(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodType);
        return utf8((u2)_info[i]);
    }

    u2 methodHandleReferenceKind(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodHandle);
        return (u2) _info[i];
    }

    u2 methodHandleReferenceIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_MethodHandle);
        return (u2) (_info[i] >> 16);
    }

    u2 invokeDynamicBootstrapMethodIndex(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return (u2) _info[i];
    }

    utf8_t *invokeDynamicMethodName(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return nameOfNameAndType((u2) (_info[i] >> 16));
    }

    utf8_t *invokeDynamicMethodType(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_InvokeDynamic);
        return typeOfNameAndType((u2) (_info[i] >> 16));
    }

    jint _int(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Integer);
        return _info[i];
    }

    void _int(u2 i, jint newInt)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Integer);
        _info[i] = newInt;
    }

    jfloat _float(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Float);
        return *(jfloat *)(_info + i);
    }

    void _float(u2 i, jfloat newFloat)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Float);
        *(jfloat *)(_info + i) = newFloat;
    }

    jlong _long(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Long);
        return *(jlong *)(_info + i);
    }

    void _long(u2 i, jlong newLong)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Long);
        *(jlong *)(_info + i) = newLong;
    }

    jdouble _double(u2 i)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Double);
        return *(jdouble *)(_info + i);
    }

    void _double(u2 i, jdouble newDouble)
    {
        assert(0 < i && i < size);
        assert(_type[i] == CONSTANT_Double);
        *(jdouble *)(_info + i) = newDouble;
    }

    Class *resolveClass(u2 i);
    Method *resolveMethod(u2 i);
    Method *resolveInterfaceMethod(u2 i);
    Field *resolveField(u2 i);
    Object *resolveString(u2 i);
    Object *resolveMethodType(u2 i);
    Object *resolveMethodHandle(u2 i);
};

#endif //KAYOVM_CONSTANTPOOL_H
