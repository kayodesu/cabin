#include <assert.h>
#include "cabin.h"
#include "object.h"
#include "constants.h"

void cp_init(ConstantPool *cp, Class *clazz, u2 size)
{
    assert(cp != NULL && clazz != NULL && size > 0);

    cp->clazz = clazz;
    cp->size = size;

    cp->type = vm_malloc(sizeof(*(cp->type)) * size);
    cp->type[0] = JVM_CONSTANT_Invalid; // constant pool 从 1 开始计数，第0位无效

    cp->info = vm_malloc(sizeof(*(cp->info)) * size);

    pthread_mutex_init(&cp->mutex, &g_pthread_mutexattr_recursive);
}

void cp_release(ConstantPool *cp)
{
    assert(cp != NULL);
    free(cp->type);
    free(cp->info);
}

#define LOCK   pthread_mutex_lock(&cp->mutex);
#define UNLOCK pthread_mutex_unlock(&cp->mutex);

u1 cp_get_type(ConstantPool *cp, u2 i)
{
    LOCK
    assert(cp != NULL);
    assert(0 < i && i < cp->size);
    u1 type = cp->type[i];
    UNLOCK
    return type;
}

void cp_set_type(ConstantPool *cp, u2 i, u1 new_type)
{
    LOCK
    assert(cp != NULL);
    assert(0 < i && i < cp->size);
    cp->type[i] = new_type;
    UNLOCK
}

void cp_set(ConstantPool *cp, u2 i, slot_t new_info)
{
    LOCK
    assert(cp != NULL);
    assert(0 < i && i < cp->size);
    cp->info[i] = new_info;
    UNLOCK
}

#define LOCK_AND_CHECK(__cp, __i, __type) \
    LOCK \
    assert(__cp != NULL); \
    assert(0 < __i && __i < __cp->size); \
    assert(__cp->type[i] == __type)

#define LOCK_AND_CHECK2(__cp, __i, __type1, __type2) \
    LOCK \
    assert(__cp != NULL); \
    assert(0 < __i && __i < __cp->size); \
    assert(__cp->type[i] == __type1 || __cp->type[i] == __type2)

utf8_t *cp_utf8(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK(cp, i, JVM_CONSTANT_Utf8);
    utf8_t *u = (utf8_t *)(cp->info[i]);
    UNLOCK
    return u;
}

u2 cp_method_handle_reference_index(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK(cp, i, JVM_CONSTANT_MethodHandle);
    u2 x = (u2) (cp->info[i] >> 16);
    UNLOCK
    return x;
}

// ----------------------------------------------------------------------------

#define CP_GET_STRING(func_name, __type)          \
    utf8_t *func_name(ConstantPool *cp, u2 i)     \
    {                                             \
        LOCK_AND_CHECK(cp, i, __type);            \
        utf8_t *u = cp_utf8(cp, (u2)cp->info[i]); \
        UNLOCK \
        return u; \
    }

CP_GET_STRING(cp_string, JVM_CONSTANT_String)
CP_GET_STRING(cp_class_name, JVM_CONSTANT_Class)
CP_GET_STRING(cp_module_name, JVM_CONSTANT_Module)
CP_GET_STRING(cp_package_name, JVM_CONSTANT_Package)
CP_GET_STRING(cp_name_of_name_and_type, JVM_CONSTANT_NameAndType)
CP_GET_STRING(cp_method_type_descriptor, JVM_CONSTANT_MethodType)

#undef CP_GET_STRING

// ----------------------------------------------------------------------------

utf8_t *cp_type_of_name_and_type(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK(cp, i, JVM_CONSTANT_NameAndType);
    utf8_t *u = cp_utf8(cp, (u2) (cp->info[i] >> 16));
    UNLOCK
    return u;
}

// ----------------------------------------------------------------------------

#define CP_GET_LOW_16_BITS(func_name, __type) \
    u2 func_name(ConstantPool *cp, u2 i)      \
    {                                         \
        LOCK_AND_CHECK(cp, i, __type);        \
        u2 x = (u2)cp->info[i];               \
        UNLOCK \
        return x; \
    }

CP_GET_LOW_16_BITS(cp_field_class_index, JVM_CONSTANT_Fieldref)
CP_GET_LOW_16_BITS(cp_method_class_index, JVM_CONSTANT_Methodref)
CP_GET_LOW_16_BITS(cp_interface_method_class_index, JVM_CONSTANT_InterfaceMethodref)
CP_GET_LOW_16_BITS(cp_method_handle_reference_kind, JVM_CONSTANT_MethodHandle)
CP_GET_LOW_16_BITS(cp_invoke_dynamic_bootstrap_method_index, JVM_CONSTANT_InvokeDynamic)

#undef CP_GET_LOW_16_BITS

// ----------------------------------------------------------------------------

#define CP_GET_MEMBER_NAME_TYPE(func_name, __type)                         \
    utf8_t *func_name##_name(ConstantPool *cp, u2 i)                       \
    {                                                                      \
        LOCK_AND_CHECK(cp, i, __type);                                     \
        utf8_t *u = cp_name_of_name_and_type(cp, (u2)(cp->info[i] >> 16)); \
        UNLOCK \
        return u; \
    } \
    \
    utf8_t *func_name##_type(ConstantPool *cp, u2 i) \
    { \
        LOCK_AND_CHECK(cp, i, __type); \
        utf8_t *u = cp_type_of_name_and_type(cp, (u2) (cp->info[i] >> 16)); \
        UNLOCK \
        return u; \
    }

#define CP_GET_MEMBER(func_name, __type) \
    utf8_t *func_name##_class_name(ConstantPool *cp, u2 i) \
    { \
        LOCK_AND_CHECK(cp, i, __type); \
        utf8_t *u = cp_class_name(cp, (u2)cp->info[i]); \
        UNLOCK \
        return u; \
    } \
    CP_GET_MEMBER_NAME_TYPE(func_name, __type)

CP_GET_MEMBER(cp_field, JVM_CONSTANT_Fieldref)
CP_GET_MEMBER(cp_method, JVM_CONSTANT_Methodref)
CP_GET_MEMBER(cp_interface_method, JVM_CONSTANT_InterfaceMethodref)
CP_GET_MEMBER_NAME_TYPE(cp_invoke_dynamic_method, JVM_CONSTANT_InvokeDynamic)

#undef CP_GET_METHOD_NAME_TYPE     
#undef CP_GET_MEMBER

// ----------------------------------------------------------------------------

#define CP_GET_SET_PRIME(jtype, prime_type, __type) \
    jtype cp_get_##prime_type(ConstantPool *cp, u2 i) \
    { \
        LOCK_AND_CHECK(cp, i, __type); \
        jtype v = slot_get_##prime_type(cp->info + i); \
        UNLOCK \
        return v; \
    } \
    \
    void cp_set_##prime_type(ConstantPool *cp, u2 i, jtype v) \
    { \
        LOCK_AND_CHECK(cp, i, __type); \
        slot_set_##prime_type(cp->info + i, v); \
        UNLOCK \
    }

CP_GET_SET_PRIME(jint, int, JVM_CONSTANT_Integer)
CP_GET_SET_PRIME(jfloat, float, JVM_CONSTANT_Float)
CP_GET_SET_PRIME(jlong, long, JVM_CONSTANT_Long)
CP_GET_SET_PRIME(jdouble, double, JVM_CONSTANT_Double)

#undef CP_GET_SET_PRIME

// ----------------------------------------------------------------------------

Class *resolve_class(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK2(cp, i, JVM_CONSTANT_Class, JVM_CONSTANT_ResolvedClass);

    Class *c;
    if (cp->type[i] == JVM_CONSTANT_ResolvedClass) {
        c = (Class *) cp->info[i];
    } else {
        c = load_class(cp->clazz->loader, cp_class_name(cp, i));
        cp_set_type(cp, i, JVM_CONSTANT_ResolvedClass);
        cp_set(cp, i, (slot_t) c);
    }

    UNLOCK
    return c;
}

Method *resolve_method(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK2(cp, i, JVM_CONSTANT_Methodref, JVM_CONSTANT_ResolvedMethod);

    Method *m;
    if (cp->type[i] == JVM_CONSTANT_ResolvedMethod) {
        m = (Method *) cp->info[i];
    } else {
        Class *c = resolve_class(cp, cp_method_class_index(cp, i));
        utf8_t *name = cp_method_name(cp, i);
        m = lookup_method(c, name, cp_method_type(cp, i));
        if (m == NULL) {
            m = get_declared_poly_method(c, name);
        }

        cp_set_type(cp, i, JVM_CONSTANT_ResolvedMethod);
        cp_set(cp, i, (slot_t) m);
    }

    UNLOCK
    return m;
}

Method *resolve_interface_method(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK2(cp, i, JVM_CONSTANT_InterfaceMethodref, JVM_CONSTANT_ResolvedInterfaceMethod);

    Method *m;
    if (cp->type[i] == JVM_CONSTANT_ResolvedInterfaceMethod) {
        m = (Method *) cp->info[i];
    } else {
        Class *c = resolve_class(cp, cp_interface_method_class_index(cp, i));
        m = lookup_method(c, cp_interface_method_name(cp, i), cp_interface_method_type(cp, i));

        cp_set_type(cp, i, JVM_CONSTANT_ResolvedInterfaceMethod);
        cp_set(cp, i, (slot_t) m);
    }

    UNLOCK
    return m;
}

Method *resolve_method_or_interface_method(ConstantPool *cp, u2 i)
{
    assert(0 < i && i < cp->size);

    if (cp->type[i] == JVM_CONSTANT_Methodref || cp->type[i] == JVM_CONSTANT_ResolvedMethod) {
        return resolve_method(cp, i);
    }
    if (cp->type[i] == JVM_CONSTANT_InterfaceMethodref || cp->type[i] == JVM_CONSTANT_ResolvedInterfaceMethod) {
        return resolve_interface_method(cp, i);
    }

    JVM_PANIC("never go here");
}

Field *resolve_field(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK2(cp, i, JVM_CONSTANT_Fieldref, JVM_CONSTANT_ResolvedField);

    Field *f;
    if (cp->type[i] == JVM_CONSTANT_ResolvedField) {
        f = (Field *) cp->info[i];
    } else {
        Class *c = resolve_class(cp, cp_field_class_index(cp, i));
        f = lookup_field(c, cp_field_name(cp, i), cp_field_type(cp, i));
        if (exception_occurred())
            return NULL;

        cp_set_type(cp, i, JVM_CONSTANT_ResolvedField);
        cp_set(cp, i, (slot_t) f);
    }

    UNLOCK
    return f;
}

Object *resolve_string(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK2(cp, i, JVM_CONSTANT_String, JVM_CONSTANT_ResolvedString);

    Object *so;
    if (cp->type[i] == JVM_CONSTANT_ResolvedString) {
        so = (Object *) cp->info[i];
    } else {
        const utf8_t *str = cp_string(cp, i);
        so = intern_string(alloc_string(str));

        cp_set_type(cp, i, JVM_CONSTANT_ResolvedString);
        cp_set(cp, i, (slot_t) so);
    }

    UNLOCK
    return so;
}

Object *resolve_method_type(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK(cp, i, JVM_CONSTANT_MethodType);
    Object *o = findMethodType(cp_method_type_descriptor(cp, i), cp->clazz->loader);
    UNLOCK
    return o;
}

Object *resolve_method_handle(ConstantPool *cp, u2 i)
{
    LOCK_AND_CHECK(cp, i, JVM_CONSTANT_MethodHandle);

    u2 kind = cp_method_handle_reference_kind(cp, i);
    u2 index = cp_method_handle_reference_index(cp, i);

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
            Field *f = resolve_field(cp, index);
            name = f->name;
            resolved_class = f->clazz;
            type_obj = get_field_type(f)->clazz->java_mirror;
            break;
        }
        case JVM_REF_invokeVirtual:
        case JVM_REF_invokeStatic:
        case JVM_REF_invokeSpecial:
        case JVM_REF_newInvokeSpecial:
        case JVM_REF_invokeInterface: {
            Method *m;
            if (kind == JVM_REF_invokeInterface) {
                m = resolve_interface_method(cp, index);
            } else {
                m = resolve_method(cp, index);
            }
            name = m->name;
            const utf8_t *desc = m->descriptor;
            resolved_class = m->clazz;
            type_obj = findMethodType(desc, m->clazz->loader);
            break;
        }
        default:
            JVM_PANIC("wrong reference kind: %d.\n", kind);
    }

    jref method_handle = linkMethodHandleConstant(cp->clazz, kind, resolved_class, name, type_obj);
    UNLOCK
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
