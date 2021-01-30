/*
 * Author: Yo Ka
 */

#include "constant_pool.h"
#include "class.h"
#include "method.h"
#include "field.h"
#include "../objects/mh.h"
#include "../interpreter/interpreter.h"

using namespace std;
using namespace slot;
using namespace method_handles;

Class *ConstantPool::resolveClass(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Class or _type[i] == JVM_CONSTANT_ResolvedClass);

    if (_type[i] == JVM_CONSTANT_ResolvedClass) {
        return (Class *) _info[i];
    }

    Class *c = loadClass(clazz->loader, className(i));
    type(i, JVM_CONSTANT_ResolvedClass);
    info(i, (slot_t) c);

    return c;
}

Method *ConstantPool::resolveMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Methodref || _type[i] == JVM_CONSTANT_ResolvedMethod);

    if (_type[i] == JVM_CONSTANT_ResolvedMethod) {
        return (Method *) _info[i];
    }

    Class *c = resolveClass(methodClassIndex(i));
    auto name = methodName(i);
    Method *m = c->lookupMethod(name, methodType(i));
    if (m == nullptr) {
        m = c->getDeclaredPolymorphicSignatureMethod(name);
    }

    type(i, JVM_CONSTANT_ResolvedMethod);
    info(i, (slot_t) m);
    return m;
}

Method* ConstantPool::resolveInterfaceMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_InterfaceMethodref
        || _type[i] == JVM_CONSTANT_ResolvedInterfaceMethod);

    if (_type[i] == JVM_CONSTANT_ResolvedInterfaceMethod) {
        return (Method *) _info[i];
    }

    Class *c = resolveClass(interfaceMethodClassIndex(i));
    Method *m = c->lookupMethod(interfaceMethodName(i), interfaceMethodType(i));

    type(i, JVM_CONSTANT_ResolvedInterfaceMethod);
    info(i, (slot_t) m);

    return m;
}

Method *ConstantPool::resolveMethodOrInterfaceMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);

    if (_type[i] == JVM_CONSTANT_Methodref || _type[i] == JVM_CONSTANT_ResolvedMethod)
        return resolveMethod(i);
    if (_type[i] == JVM_CONSTANT_InterfaceMethodref || _type[i] == JVM_CONSTANT_ResolvedInterfaceMethod)
        return resolveInterfaceMethod(i);

    jvm_abort("never go here");
}

Field *ConstantPool::resolveField(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_Fieldref or _type[i] == JVM_CONSTANT_ResolvedField);

    if (_type[i] == JVM_CONSTANT_ResolvedField) {
        return (Field *) _info[i];
    }

    Class *c = resolveClass(fieldClassIndex(i));
    Field *f = c->lookupField(fieldName(i), fieldType(i));

    type(i, JVM_CONSTANT_ResolvedField);
    info(i, (slot_t) f);

    return f;
}

Object *ConstantPool::resolveString(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_String or _type[i] == JVM_CONSTANT_ResolvedString);

    if (_type[i] == JVM_CONSTANT_ResolvedString) {
        return (Object *) _info[i];
    }

    const utf8_t *str = string(i);
    Object *so = g_string_class->intern(str);

    type(i, JVM_CONSTANT_ResolvedString);
    info(i, (slot_t) so);
    return so;
}

Object *ConstantPool::resolveMethodType(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_MethodType);
    return findMethodType(methodTypeDescriptor(i), clazz->loader);
}

Object *ConstantPool::resolveMethodHandle(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(_type[i] == JVM_CONSTANT_MethodHandle);

    auto caller = getCaller();

    u2 kind = methodHandleReferenceKind(i);
    u2 index = methodHandleReferenceIndex(i);

    const utf8_t *d1 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;)"
                       "Ljava/lang/invoke/MethodHandle;";
    const utf8_t *d2 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)"
                       "Ljava/lang/invoke/MethodHandle;";
    const utf8_t *d3 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/Class;)"
                       "Ljava/lang/invoke/MethodHandle;";

    switch (kind) {
        case JVM_REF_getField: {
            Field *f = resolveField(index);

            // public MethodHandle findGetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findGetter", d1);
            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
        }
        case JVM_REF_getStatic: {
            Field *f = resolveField(index);

            // public MethodHandle findStaticGetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findStaticGetter", d1);
            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
        }
        case JVM_REF_putField: {
            Field *f = resolveField(index);

            // public MethodHandle findSetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findSetter", d1);
            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
        }
        case JVM_REF_putStatic: {
            Field *f = resolveField(index);

            // public MethodHandle findStaticSetter(Class<?> refc, String name, Class<?> type)
            //                      throws NoSuchFieldException, IllegalAccessException;
            Method *m = caller->clazz->getDeclaredInstMethod("findStaticSetter", d1);
            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
        }
        case JVM_REF_invokeVirtual :{
            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("not implement.");
        }
        case JVM_REF_invokeStatic: {
            Method *m = resolveMethod(index);
            assert(m->isStatic());

            jref mt = findMethodType(m->descriptor, m->clazz->loader);

            Class *mthd_hndl_natives_class = loadBootClass(S(java_lang_invoke_MethodHandleNatives));
            // static MethodHandle linkMethodHandleConstant(Class<?> callerClass, int refKind, 
            //                                                  Class<?> defc, String name, Object type)
            Method *m0 = mthd_hndl_natives_class->getDeclaredStaticMethod("linkMethodHandleConstant", "(Ljava/lang/Class;ILjava/lang/Class;Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/invoke/MethodHandle;");
            return getRef(execJavaFunc(m0, { rslot(clazz->java_mirror), islot(kind), rslot(m->clazz->java_mirror), rslot(newString(m->name)), rslot(mt) }));
            // // public MethodHandle findStatic(Class<?> refc, String name, MethodType type)
            // //                      throws NoSuchMethodException, IllegalAccessException;
            // Method *m0 = caller->clazz->getDeclaredInstMethod("findStatic", d2);
            // return RSLOT(execJavaFunc(m0, { caller, m->clazz->java_mirror, newString(m->name), mt }));
        }
        case JVM_REF_invokeSpecial: {
            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("not implement.");
        }
        case JVM_REF_newInvokeSpecial: {
            // public MethodHandle findConstructor(Class<?> refc, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;

            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
            //                      throws NoSuchMethodException, IllegalAccessException;
           jvm_abort("not implement.");
        }
        case JVM_REF_invokeInterface: {
            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
            //                      throws NoSuchMethodException, IllegalAccessException;
            jvm_abort("not implement.");
        }
        default:
            jvm_abort("wrong reference kind: %d.\n", kind);
    }

    // todo
    jvm_abort("xxx");
}
