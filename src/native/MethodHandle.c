#include "jni_internal.h"
#include "../cabin.h"
#include "../runtime/frame.h"
#include "../metadata/descriptor.h"
#include "../objects/object.h"
#include "../interpreter/interpreter.h"


/**
 * Invokes the method handle, allowing any caller type type, but requiring an exact type match.
 * The symbolic type type at the call site of {@code invokeExact} must
 * exactly match this method handle's {@link #type type}.
 * No conversions are allowed on arguments or return values.
 * <p>
 * When this method is observed via the Core Reflection API,
 * it will appear as a single native method, taking an object array and returning an object.
 * If this native method is invoked directly via
 * {@link java.lang.reflect.Method#invoke java.lang.reflect.Method.invoke}, via JNI,
 * or indirectly via {@link java.lang.invoke.MethodHandles.Lookup#unreflect Lookup.unreflect},
 * it will throw an {@code UnsupportedOperationException}.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 * @throws WrongMethodTypeException if the target's type is not identical with the caller's symbolic type type
 * @throws Throwable anything thrown by the underlying method propagates unchanged through the method handle call
 */
// public final native @PolymorphicSignature Object invokeExact(Object... args) throws Throwable;
static jref invokeExact(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle
;
    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
}

/**
 * Invokes the method handle, allowing any caller type type,
 * and optionally performing conversions on arguments and return values.
 * <p>
 * If the call site's symbolic type type exactly matches this method handle's {@link #type type},
 * the call proceeds as if by {@link #invokeExact invokeExact}.
 * <p>
 * Otherwise, the call proceeds as if this method handle were first
 * adjusted by calling {@link #asType asType} to adjust this method handle
 * to the required type, and then the call proceeds as if by
 * {@link #invokeExact invokeExact} on the adjusted method handle.
 * <p>
 * There is no guarantee that the {@code asType} call is actually made.
 * If the JVM can predict the results of making the call, it may perform
 * adaptations directly on the caller's arguments,
 * and call the target method handle according to its own exact type.
 * <p>
 * The resolved type type at the call site of {@code invoke} must
 * be a valid argument to the receivers {@code asType} method.
 * In particular, the caller must specify the same argument arity
 * as the callee's type,
 * if the callee is not a {@linkplain #asVarargsCollector variable arity collector}.
 * <p>
 * When this method is observed via the Core Reflection API,
 * it will appear as a single native method, taking an object array and returning an object.
 * If this native method is invoked directly via
 * {@link java.lang.reflect.Method#invoke java.lang.reflect.Method.invoke}, via JNI,
 * or indirectly via {@link java.lang.invoke.MethodHandles.Lookup#unreflect Lookup.unreflect},
 * it will throw an {@code UnsupportedOperationException}.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 * @throws WrongMethodTypeException if the target's type cannot be adjusted to the caller's symbolic type type
 * @throws ClassCastException if the target's type can be adjusted to the caller, but a reference cast fails
 * @throws Throwable anything thrown by the underlying method propagates unchanged through the method handle call
 */
// public final native @PolymorphicSignature Object invoke(Object... args) throws Throwable;
static jref invoke(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle

    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);

#if 0
    // MemberName internalMemberName();
    Method *m = _this->clazz->lookupInstMethod("internalMemberName", "()Ljava/lang/invoke/MemberName;");
    jref member_name = getRef(execJavaFunc(m, {_this}));
    assert(member_name != NULL);
    // private Class<?> clazz;       // class in which the method is defined
    // private String   name;        // may be null if not yet materialized
    // private Object   type;        // may be null if not yet materialized
    Class *c = member_name->getRefField<ClsObj>(S(clazz), S(sig_java_lang_Class))->jvm_mirror;
    auto name = member_name->getRefField(S(name), S(sig_java_lang_String))->toUtf8();

    // public MethodType getInvocationType()
    m = member_name->clazz->lookupInstMethod("getInvocationType", "()Ljava/lang/invoke/MethodType;");
    jref method_type = getRef(execJavaFunc(m, {member_name}));
    string desc = unparseMethodDescriptor(method_type);
    // todo 判断desc，如果有基本类型参数，则参数值要进行unbox。

//    int num = numElementsInMethodDescriptor(desc.c_str());
//    slot_t args[num];
//
//    va_list ap;
//    va_start(ap, _this);
//    for (int i = 0; i < num; i++) {
//        Object *o = va_arg(ap, Object *);
//        args[i] = rslot(o);
//    }
//    va_end(ap);

    m = c->lookupMethod(name, desc.c_str());
    slot_t *slot = execJavaFunc(m, args);
    return getRef(slot);
#endif
}
//static jref invoke(jref _this, ...)
//{
//    assert(_this != NULL);
//    // _this is a object of subclass of java.lang.invoke.MethodHandle
//
//    // MemberName internalMemberName();
//    Method *m = _this->clazz->lookupInstMethod("internalMemberName", "()Ljava/lang/invoke/MemberName;");
//    jref member_name = getRef(execJavaFunc(m, {_this}));
//    // private Class<?> clazz;       // class in which the method is defined
//    // private String   name;        // may be null if not yet materialized
//    // private Object   type;        // may be null if not yet materialized
//    Class *c = member_name->getRefField<ClsObj>(S(clazz), S(sig_java_lang_Class))->jvm_mirror;
//    auto name = member_name->getRefField(S(name), S(sig_java_lang_String))->toUtf8();
//
//    // public MethodType getInvocationType()
//    m = member_name->clazz->lookupInstMethod("getInvocationType", "()Ljava/lang/invoke/MethodType;");
//    jref method_type = getRef(execJavaFunc(m, {member_name}));
//    string desc = unparseMethodDescriptor(method_type);
//    // todo 判断desc，如果有基本类型参数，则参数值要进行unbox。
//
//    int num = numElementsInMethodDescriptor(desc.c_str());
//    slot_t args[num];
//
//    va_list ap;
//    va_start(ap, _this);
//    for (int i = 0; i < num; i++) {
//        Object *o = va_arg(ap, Object *);
//        args[i] = rslot(o);
//    }
//    va_end(ap);
//
//    m = c->lookupMethod(name, desc.c_str());
//    slot_t *slot = execJavaFunc(m, args);
//    return getRef(slot);
//}

/**
 * Private method for trusted invocation of a method handle respecting simplified signatures.
 * Type mismatches will not throw {@code WrongMethodTypeException}, but could crash the JVM.
 * <p>
 * The caller signature is restricted to the following basic types:
 * Object, int, long, float, double, and void return.
 * <p>
 * The caller is responsible for maintaining type correctness by ensuring
 * that the each outgoing argument value is a member of the range of the corresponding
 * callee argument type.
 * (The caller should therefore issue appropriate casts and integer narrowing
 * operations on outgoing argument values.)
 * The caller can assume that the incoming result value is part of the range
 * of the callee's return type.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// final native @PolymorphicSignature Object invokeBasic(Object... args) throws Throwable;
static jref invokeBasic(const slot_t *args)
{
    assert(args != NULL);
    jref _this = slot_get_ref(args);
    assert(_this != NULL);
    // _this is a object of subclass of java.lang.invoke.MethodHandle

    jref form = get_ref_field(_this, S(form), "Ljava/lang/invoke/LambdaForm;");
    jref entry = get_ref_field(form, S(vmentry), "Ljava/lang/invoke/MemberName;");
    Method *target = (Method *) (void *) get_ref_field(entry, S(vmtarget), S(sig_java_lang_Object));

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeVirtual}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToVirtual(Object... args) throws Throwable;
static jref linkToVirtual(u2 args_slots_count, const slot_t *args)
{
    JVM_PANIC("linkToVirtual");
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeStatic}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToStatic(Object... args) throws Throwable;
static jref linkToStatic(u2 args_slots_count, const slot_t *args)
{
    Object *member_name = slot_get_ref(args + args_slots_count - 1);
    Method *target = (Method *) (void *) get_ref_field(member_name, S(vmtarget), S(sig_java_lang_Object));
    assert(target != NULL);

    slot_t *slot = exec_java_func(target, args);
    return slot_get_ref(slot);
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeSpecial}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToSpecial(Object... args) throws Throwable;
static jref linkToSpecial(u2 args_slots_count, const slot_t *args)
{
    JVM_PANIC("linkToSpecial");
}

/**
 * Private method for trusted invocation of a MemberName of kind {@code REF_invokeInterface}.
 * The caller signature is restricted to basic types as with {@code invokeBasic}.
 * The trailing (not leading) argument must be a MemberName.
 * @param args the signature-polymorphic parameter list, statically represented using varargs
 * @return the signature-polymorphic result, statically represented using {@code Object}
 */
// static native @PolymorphicSignature Object linkToInterface(Object... args) throws Throwable;
static jref linkToInterface(jref args)
{
    JVM_PANIC("linkToInterface");
}

#undef T
#define T "([Ljava/lang/Object;)Ljava/lang/Object;"

static JNINativeMethod methods[] = {
        JNINativeMethod_registerNatives,
        { "invokeExact",     T, invokeExact },
        { "invoke",          T, invoke },
        { "invokeBasic",     T, invokeBasic },
        { "linkToVirtual",   T, linkToVirtual },
        { "linkToStatic",    T, linkToStatic },
        { "linkToSpecial",   T, linkToSpecial },
        { "linkToInterface", T, linkToInterface },
        { NULL }
};
#undef T

void java_lang_invoke_MethodHandle_registerNatives()
{
    register_natives("java/lang/invoke/MethodHandle", methods);
}
