/*
 * Author: kayo
 */

#include "invoke.h"
#include "Class.h"
#include "Array.h"
#include "Method.h"
#include "../interpreter/interpreter.h"

using namespace method_type;

jref method_type::fromMethodDescriptor(const utf8_t *descriptor, jref loader)
{
    assert(descriptor != nullptr);

    Class *mt = loadBootClass("java/lang/invoke/MethodType");
    // public static MethodType fromMethodDescriptorString(String descriptor, ClassLoader loader)
    //                      throws IllegalArgumentException, TypeNotPresentException
    Method *m = mt->getDeclaredStaticMethod(
            "fromMethodDescriptorString",
            "(Ljava/lang/String;Ljava/lang/ClassLoader;)Ljava/lang/invoke/MethodType;");
    return (jref) *execJavaFunc(m, newString(descriptor), loader);
}

jref method_type::methodType(Method *m)
{
    assert(m != nullptr);
    return m->getType();
}

jref method_type::methodType(jclsref rtype, jarrref ptypes)
{
    assert(rtype != nullptr);
    assert(ptypes != nullptr);

    Class *mt = loadBootClass("java/lang/invoke/MethodType");
    // public static MethodType methodType(Class<?> rtype, Class<?>[] ptypes);
    Method *m = mt->getDeclaredStaticMethod(
            "methodType", "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
    return (jref) *execJavaFunc(m, rtype, ptypes);
}

jstrref method_type::toMethodDescriptor(jref methodType)
{
    assert(methodType != nullptr);

    Class *mt = loadBootClass("java/lang/invoke/MethodType");
    // public String toMethodDescriptorString();
    Method *m = mt->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
    return (jstrref) *execJavaFunc(m, methodType);
}

/* ----------------------------------------------------------------------------------------- */

jref member_name::memberName(Method *m, jbyte refKind)
{
    assert(m != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public MemberName(Class<?> defClass, String name, MethodType type, byte refKind);
    auto cons = mn->getConstructor("(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;B)V");
    jref o = newObject(mn);
    execJavaFunc(cons, { (slot_t) o, (slot_t) m->clazz, (slot_t) newString(m->name), (slot_t) methodType(m), refKind });
    return o;
}

jbyte member_name::getRefKind(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public byte getReferenceKind();
    auto m = mn->getDeclaredInstMethod("getReferenceKind", "()B");
    return (jbyte) *execJavaFunc(m, memberName);
}

bool member_name::isMethod(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isMethod();
    auto m = mn->getDeclaredInstMethod("isMethod", "()Z");
    slot_t *ret = execJavaFunc(m, memberName);
    return (jint) *ret != 0;
}

bool member_name::isConstructor(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isConstructor();
    auto m = mn->getDeclaredInstMethod("isConstructor", "()Z");
    slot_t *ret = execJavaFunc(m, memberName);
    return (jint) *ret != 0;
}

bool member_name::isField(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isField();
    auto m = mn->getDeclaredInstMethod("isField", "()Z");
    slot_t *ret = execJavaFunc(m, memberName);
    return (jint) *ret != 0;
}

bool member_name::isType(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isType();
    auto m = mn->getDeclaredInstMethod("isType", "()Z");
    slot_t *ret = execJavaFunc(m, memberName);
    return (jint) *ret != 0;
}

bool member_name::isStatic(jref memberName)
{
    assert(memberName != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isStatic();
    auto m = mn->getDeclaredInstMethod("isStatic", "()Z");
    slot_t *ret = execJavaFunc(m, memberName);
    return (jint) *ret != 0;
}
/* ----------------------------------------------------------------------------------------- */

jref method_handles::getCaller()
{
    // public static Lookup lookup();
    Class *mh = loadBootClass("java/lang/invoke/MethodHandles");
    auto lookup = mh->getDeclaredStaticMethod("lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
    return (jref) *execJavaFunc(lookup);
}