#include <iostream>
#include "constant_pool.h"
#include "class.h"
#include "method.h"
#include "../objects/mh.h"
#include "../interpreter/interpreter.h"

using namespace std;
using namespace slot;
using namespace method_handles;

Class *ConstantPool::resolveClass(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_Class or type[i] == JVM_CONSTANT_ResolvedClass);

    if (type[i] == JVM_CONSTANT_ResolvedClass) {
        return (Class *) info[i];
    }

    Class *c = loadClass(clazz->loader, className(i));
    setType(i, JVM_CONSTANT_ResolvedClass);
    setInfo(i, (slot_t) c);

    return c;
}

Method *ConstantPool::resolveMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_Methodref || type[i] == JVM_CONSTANT_ResolvedMethod);

    if (type[i] == JVM_CONSTANT_ResolvedMethod) {
        return (Method *) info[i];
    }

    Class *c = resolveClass(methodClassIndex(i));
    auto name = methodName(i);
    Method *m = c->lookupMethod(name, methodType(i));
    if (m == nullptr) {
        m = c->getDeclaredPolymorphicSignatureMethod(name);
    }

    setType(i, JVM_CONSTANT_ResolvedMethod);
    setInfo(i, (slot_t) m);
    return m;
}

Method* ConstantPool::resolveInterfaceMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_InterfaceMethodref
           || type[i] == JVM_CONSTANT_ResolvedInterfaceMethod);

    if (type[i] == JVM_CONSTANT_ResolvedInterfaceMethod) {
        return (Method *) info[i];
    }

    Class *c = resolveClass(interfaceMethodClassIndex(i));
    Method *m = c->lookupMethod(interfaceMethodName(i), interfaceMethodType(i));

    setType(i, JVM_CONSTANT_ResolvedInterfaceMethod);
    setInfo(i, (slot_t) m);

    return m;
}

Method *ConstantPool::resolveMethodOrInterfaceMethod(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);

    if (type[i] == JVM_CONSTANT_Methodref || type[i] == JVM_CONSTANT_ResolvedMethod)
        return resolveMethod(i);
    if (type[i] == JVM_CONSTANT_InterfaceMethodref || type[i] == JVM_CONSTANT_ResolvedInterfaceMethod)
        return resolveInterfaceMethod(i);

    JVM_PANIC("never go here");
}

Field *ConstantPool::resolveField(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_Fieldref or type[i] == JVM_CONSTANT_ResolvedField);

    if (type[i] == JVM_CONSTANT_ResolvedField) {
        return (Field *) info[i];
    }

    Class *c = resolveClass(fieldClassIndex(i));
    Field *f = c->lookupField(fieldName(i), fieldType(i));

    setType(i, JVM_CONSTANT_ResolvedField);
    setInfo(i, (slot_t) f);

    return f;
}

Object *ConstantPool::resolveString(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_String or type[i] == JVM_CONSTANT_ResolvedString);

    if (type[i] == JVM_CONSTANT_ResolvedString) {
        return (Object *) info[i];
    }

    const utf8_t *str = string(i);
    Object *so = g_string_class->intern(str);

    setType(i, JVM_CONSTANT_ResolvedString);
    setInfo(i, (slot_t) so);
    return so;
}

Object *ConstantPool::resolveMethodType(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_MethodType);
    return findMethodType(methodTypeDescriptor(i), clazz->loader);
}

Object *ConstantPool::resolveMethodHandle(u2 i)
{
    lock_guard<recursive_mutex> lock(mutex);
    assert(0 < i && i < size);
    assert(type[i] == JVM_CONSTANT_MethodHandle);

    u2 kind = methodHandleReferenceKind(i);
    u2 index = methodHandleReferenceIndex(i);

//    auto caller = getCaller();
//    const utf8_t *d1 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/Class;)"
//                       "Ljava/lang/invoke/MethodHandle;";
//    const utf8_t *d2 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;)"
//                       "Ljava/lang/invoke/MethodHandle;";
//    const utf8_t *d3 = "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;Ljava/lang/Class;)"
//                       "Ljava/lang/invoke/MethodHandle;";
//    const utf8_t *d4 = "(Ljava/lang/Class;ILjava/lang/Class;Ljava/lang/String;Ljava/lang/Object;)"
//                       "Ljava/lang/invoke/MethodHandle;";

    const char *name;
    Class *resolved_class;
    Object *type_obj;

    switch (kind) {
        case JVM_REF_getField:
        case JVM_REF_getStatic:
        case JVM_REF_putField:
        case JVM_REF_putStatic: {
            Field *f = resolveField(index);
            name = f->name;
            resolved_class = f->clazz;
            type_obj = f->getType()->clazz->java_mirror;
            break;
        }
        case JVM_REF_invokeVirtual:
        case JVM_REF_invokeStatic:
        case JVM_REF_invokeSpecial:
        case JVM_REF_newInvokeSpecial:
        case JVM_REF_invokeInterface: {
            Method *m;
            if (kind == JVM_REF_invokeInterface) {
                m = resolveInterfaceMethod(index);
            } else {
                m = resolveMethod(index);
            }
            name = m->name;
            auto desc = m->descriptor;
            resolved_class = m->clazz;
            type_obj = findMethodType(desc, m->clazz->loader);
            break;
        }
        default:
            JVM_PANIC("wrong reference kind: %d.\n", kind);
    }

    jref method_handle = linkMethodHandleConstant(clazz, kind, resolved_class, name, type_obj);
    return method_handle;

//    switch (kind) {
//        case JVM_REF_getField: {
//            Field *f = resolveField(index);
//
//            // public MethodHandle findGetter(Class<?> refc, String name, Class<?> type)
//            //                      throws NoSuchFieldException, IllegalAccessException;
//            Method *m = caller->clazz->getDeclaredInstMethod("findGetter", d1);
//            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
//        }
//        case JVM_REF_getStatic: {
//            Field *f = resolveField(index);
//
//            // public MethodHandle findStaticGetter(Class<?> refc, String name, Class<?> type)
//            //                      throws NoSuchFieldException, IllegalAccessException;
//            Method *m = caller->clazz->getDeclaredInstMethod("findStaticGetter", d1);
//            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
//        }
//        case JVM_REF_putField: {
//            Field *f = resolveField(index);
//
//            // public MethodHandle findSetter(Class<?> refc, String name, Class<?> type)
//            //                      throws NoSuchFieldException, IllegalAccessException;
//            Method *m = caller->clazz->getDeclaredInstMethod("findSetter", d1);
//            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
//        }
//        case JVM_REF_putStatic: {
//            Field *f = resolveField(index);
//
//            // public MethodHandle findStaticSetter(Class<?> refc, String name, Class<?> type)
//            //                      throws NoSuchFieldException, IllegalAccessException;
//            Method *m = caller->clazz->getDeclaredInstMethod("findStaticSetter", d1);
//            return getRef(execJavaFunc(m, { caller, f->clazz->java_mirror, newString(f->name), f->getType() }));
//        }
//        case JVM_REF_invokeVirtual :{
//            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
//            //                      throws NoSuchMethodException, IllegalAccessException;
//            JVM_PANIC("not implement.");
//        }
//        case JVM_REF_invokeStatic: {
//            Method *m = resolveMethod(index);
//            assert(m->isStatic());
//
//            jref mt = findMethodType(m->descriptor, m->clazz->loader);
//
//            Class *c = loadBootClass(S(java_lang_invoke_MethodHandleNatives));
//            // static MethodHandle linkMethodHandleConstant(Class<?> callerClass, int refKind,
//            //                                                  Class<?> defc, String name, Object type)
//            Method *m0 = c->getDeclaredStaticMethod("linkMethodHandleConstant", d4);
//            return getRef(execJavaFunc(m0, { rslot(clazz->java_mirror), islot(kind), rslot(m->clazz->java_mirror), rslot(newString(m->name)), rslot(mt) }));
//            // // public MethodHandle findStatic(Class<?> refc, String name, MethodType type)
//            // //                      throws NoSuchMethodException, IllegalAccessException;
//            // Method *m0 = caller->clazz->getDeclaredInstMethod("findStatic", d2);
//            // return RSLOT(execJavaFunc(m0, { caller, m->clazz->java_mirror, newString(m->name), mt }));
//        }
//        case JVM_REF_invokeSpecial: {
//            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
//            //                      throws NoSuchMethodException, IllegalAccessException;
//            JVM_PANIC("not implement.");
//        }
//        case JVM_REF_newInvokeSpecial: {
//            // public MethodHandle findConstructor(Class<?> refc, MethodType type)
//            //                      throws NoSuchMethodException, IllegalAccessException;
//
//            // public MethodHandle findSpecial(Class<?> refc, String name, MethodType type, Class<?> specialCaller)
//            //                      throws NoSuchMethodException, IllegalAccessException;
//           JVM_PANIC("not implement.");
//        }
//        case JVM_REF_invokeInterface: {
//            // public MethodHandle findVirtual(Class<?> refc, String name, MethodType type)
//            //                      throws NoSuchMethodException, IllegalAccessException;
//            JVM_PANIC("not implement.");
//        }
//        default:
//            JVM_PANIC("wrong reference kind: %d.\n", kind);
//    }
}
