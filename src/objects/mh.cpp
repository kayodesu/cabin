/*
 * Author: Yo Ka
 */

#include "../symbol.h"
#include "../slot.h"
#include "mh.h"
#include "../metadata/class.h"
#include "../metadata/method.h"
#include "array_object.h"
#include "class_object.h"
#include "../interpreter/interpreter.h"
#include "../metadata/descriptor.h"
#include "../runtime/vm_thread.h"

using namespace std;
using namespace method_type;
using namespace slot;
using namespace utf8;

static Class *constructor_reflect_class;
static Class *method_reflect_class;
static Class *field_reflect_class;

static Class *member_name_class;
static Field *mn_clazz_field;
static Field *mn_name_field;
static Field *mn_type_field;
static Field *mn_flags_field;
static Method *mn_getSignature_method;

void initMethodHandle()
{
    constructor_reflect_class = loadBootClass(S(java_lang_reflect_Constructor));
    method_reflect_class = loadBootClass(S(java_lang_reflect_Method));
    field_reflect_class = loadBootClass(S(java_lang_reflect_Field));

    Class *c = member_name_class = loadBootClass("java/lang/invoke/MemberName");
    // private Class<?> clazz;
    mn_clazz_field = c->getDeclaredField(S(clazz), S(sig_java_lang_Class));
    // private String name;
    mn_name_field = c->getDeclaredField(S(name), S(sig_java_lang_String));
    // private Object type;
    mn_type_field = c->getDeclaredField(S(type), S(sig_java_lang_Object));
    // private int flags;
    mn_flags_field = c->getDeclaredField(S(flags), S(I));
    // public String getSignature();
    mn_getSignature_method = c->getDeclaredInstMethod("getSignature", S(___java_lang_String));

    if (constructor_reflect_class == nullptr || method_reflect_class == nullptr
        || field_reflect_class == nullptr || member_name_class == nullptr
        || mn_clazz_field == nullptr || mn_name_field == nullptr || mn_type_field == nullptr
        || mn_flags_field == nullptr || mn_getSignature_method == nullptr) {
        throw new runtime_error("initMethodHandle"); // todo
    }
}

jref findMethodType(const utf8_t *desc, jref loader)
{
    assert(desc != nullptr);

    pair<Array *, ClassObject *> p = parseMethodDescriptor(desc, loader);
    if (exceptionOccured())
        return nullptr;

    return findMethodType(p.second, p.first);
}

jref findMethodType(ClassObject *rtype, Array *ptypes)
{
    assert(rtype != nullptr && ptypes != nullptr);

    Class *mhn = loadBootClass(S(java_lang_invoke_MethodHandleNatives));
    // static MethodType findMethodHandleType(Class<?> rtype, Class<?>[] ptypes)
    Method *m = mhn->getDeclaredStaticMethod("findMethodHandleType",
                "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
    return RSLOT(execJavaFunc(m, { rtype, ptypes }));
}

Array *method_type::parameterTypes(jref methodType)
{
    assert(methodType != nullptr);

    // private final Class<?>[] ptypes;
    auto ptypes = methodType->getRefField<Array>("ptypes", "[Ljava/lang/Class;");
    return ptypes;
}

jstrref method_type::toMethodDescriptor(jref methodType)
{
    assert(methodType != nullptr);

    Class *mt = loadBootClass("java/lang/invoke/MethodType");
    // public String toMethodDescriptorString();
    Method *m = mt->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
    return (jstrref) RSLOT(execJavaFunc(m, {methodType}));
}

/* ----------------------------------------------------------------------------------------- */

jref member_name::memberName(Method *m, jbyte refKind)
{
    assert(m != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public MemberName(Class<?> defClass, String name, MethodType type, byte refKind);
    auto cons = mn->getConstructor("(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;B)V");
    jref o = newObject(mn);
    jref mt = findMethodType(m->descriptor, m->clazz->loader);
    execJavaFunc(cons, { rslot(o), rslot(m->clazz->java_mirror), rslot(newString(m->name)),
                         rslot(mt), islot(refKind) });
    return o;
}

jbyte member_name::getRefKind(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public byte getReferenceKind();
    auto m = mn->getDeclaredInstMethod("getReferenceKind", "()B");
    return (jbyte) ISLOT(execJavaFunc(m, {member_name}));
}

bool member_name::isMethod(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isMethod();
    auto m = mn->getDeclaredInstMethod("isMethod", "()Z");
    return ISLOT(execJavaFunc(m, {member_name})) != 0;
}

bool member_name::isConstructor(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isConstructor();
    auto m = mn->getDeclaredInstMethod("isConstructor", "()Z");
    return ISLOT(execJavaFunc(m, {member_name})) != 0;
}

bool member_name::isField(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isField();
    auto m = mn->getDeclaredInstMethod("isField", "()Z");
    return ISLOT(execJavaFunc(m, {member_name})) != 0;
}

bool member_name::isType(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isType();
    auto m = mn->getDeclaredInstMethod("isType", "()Z");
    return ISLOT(execJavaFunc(m, {member_name})) != 0;
}

bool member_name::isStatic(jref member_name)
{
    assert(member_name != nullptr);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isStatic();
    auto m = mn->getDeclaredInstMethod("isStatic", "()Z");
    return ISLOT(execJavaFunc(m, {member_name})) != 0;
}

/* Method flags */

#define MB_LAMBDA_HIDDEN        1
#define MB_LAMBDA_COMPILED      2
#define MB_CALLER_SENSITIVE     4
#define MB_DEFAULT_CONFLICT     8


#define IS_METHOD        0x010000
#define IS_CONSTRUCTOR   0x020000
#define IS_FIELD         0x040000
#define IS_TYPE          0x080000
#define CALLER_SENSITIVE 0x100000

#define SEARCH_SUPERCLASSES 0x100000
#define SEARCH_INTERFACES   0x200000

#define ALL_KINDS (IS_METHOD | IS_CONSTRUCTOR | IS_FIELD | IS_TYPE)

#define REFERENCE_KIND_SHIFT 24
#define REFERENCE_KIND_MASK  (0xf000000 >> REFERENCE_KIND_SHIFT)

static int methodFlags(Method *m)
{
    int flags = m->access_flags;

    if(m->access_flags & MB_CALLER_SENSITIVE)
        flags |= CALLER_SENSITIVE;

    return flags;
}

void initMemberName(jref member_name, jref target)
{
    assert(member_name != nullptr && target != nullptr);

    /*
     * in fact, `target` will be one of these three:
     * 1. java/lang/reflect/Field.
     * 2. java/lang/reflect/Constructor.
     * 3. java/lang/reflect/Method.
     */

    if (target->clazz == method_reflect_class) {
        // private Class<?> clazz;
        Class *decl_class = target->getRefField<ClassObject>("clazz", "Ljava/lang/Class;")->jvm_mirror;
        // private int slot;
        int slot = target->getIntField("slot", "I");

        Method *m = decl_class->methods[slot];
        int flags = methodFlags(m) | IS_METHOD;

        int ref_kind;
        if (m->isStatic()) {
            ref_kind = JVM_REF_invokeStatic;
        } else if (decl_class->isInterface()) {
            ref_kind = JVM_REF_invokeInterface;
        } else {
            ref_kind = JVM_REF_invokeVirtual;
        }

        flags |= ref_kind << REFERENCE_KIND_SHIFT;

        member_name->setRefField(mn_clazz_field, decl_class->java_mirror);
        member_name->setIntField(mn_flags_field, flags);
        return;

//        int slot = INST_DATA(target, int, mthd_slot_offset);
//        Class *decl_class = INST_DATA(target, Class*, mthd_class_offset);
//
//        ClassBlock *cb = CLASS_CB(decl_class);
//        MethodBlock *mb = &(cb->methods[slot]);
//        int flags = mbFlags(mb) | IS_METHOD;
//        int ref_kind;
//
//        if(mb->access_flags & ACC_STATIC)
//            ref_kind = REF_invokeStatic;
//        else if(IS_INTERFACE(cb))
//            ref_kind = REF_invokeInterface;
//        else
//            ref_kind = REF_invokeVirtual;
//
//        flags |= ref_kind << REFERENCE_KIND_SHIFT;
//
//        INST_DATA(mname, Class*, mem_name_clazz_offset) = decl_class;
//        INST_DATA(mname, int, mem_name_flags_offset) = flags;
//        INST_DATA(mname, MethodBlock*, mem_name_vmtarget_offset) = mb;
    }

    if (target->clazz == constructor_reflect_class) {
//        // private Class<?> clazz;
//        Class *decl_class = target->getRefField<ClassObject>("clazz", "Ljava/lang/Class;")->jvm_mirror;
//        // private int slot;
//        int slot = target->getIntField("slot", "I");
//
//        Method *m = decl_class->methods[slot];
//        int flags = methodFlags(m) | IS_CONSTRUCTOR | (JVM_REF_invokeSpecial << REFERENCE_KIND_SHIFT);
//
//        member_name->setRefField("clazz", "Ljava/lang/Class;", decl_class->java_mirror);
//        member_name->setIntField("flags", "I", flags);

//        int slot = INST_DATA(target, int, cons_slot_offset);
//        Class *decl_class = INST_DATA(target, Class*, cons_class_offset);
//        MethodBlock *mb = &(CLASS_CB(decl_class)->methods[slot]);
//        int flags = mbFlags(mb) | IS_CONSTRUCTOR |
//                    (REF_invokeSpecial << REFERENCE_KIND_SHIFT);
//
//        INST_DATA(mname, Class*, mem_name_clazz_offset) = decl_class;
//        INST_DATA(mname, int, mem_name_flags_offset) = flags;
//        INST_DATA(mname, MethodBlock*, mem_name_vmtarget_offset) = mb;
        jvm_abort("constructor_reflect_class");
        return;
    }

    if (target->clazz == field_reflect_class) {
        // private Class<?> clazz;
//        Class *decl_class = target->getRefField<ClassObject>("clazz", "Ljava/lang/Class;")->jvm_mirror;
//        // private int slot;
//        int slot = target->getIntField("slot", "I");
//
//        Field *f = decl_class->fields + slot;
//        int flags = f->access_flags | IS_FIELD;
//        if (f->isStatic()) {
//            flags |= JVM_REF_getStatic << REFERENCE_KIND_SHIFT;
//        } else {
//            flags |= JVM_REF_getField << REFERENCE_KIND_SHIFT;
//        }
//
//        member_name->setRefField("clazz", "Ljava/lang/Class;", decl_class->java_mirror);
//        member_name->setIntField("flags", "I", flags);

//        Class *decl_class = INST_DATA(target, Class*, fld_class_offset);
//        int slot = INST_DATA(target, int, fld_slot_offset);
//        FieldBlock *fb = &(CLASS_CB(decl_class)->fields[slot]);
//        int flags = fb->access_flags | IS_FIELD;
//
//        flags |= (fb->access_flags & ACC_STATIC ? REF_getStatic
//                                                : REF_getField)
//                << REFERENCE_KIND_SHIFT;
//
//        INST_DATA(mname, Class*, mem_name_clazz_offset) = decl_class;
//        INST_DATA(mname, int, mem_name_flags_offset) = flags;
//        INST_DATA(mname, FieldBlock*, mem_name_vmtarget_offset) = fb;
        jvm_abort("field_reflect_class");
        return;
    }
    jvm_abort("initMemberName: unimplemented target"); // todo
    signalException(S(java_lang_InternalError), "initMemberName: unimplemented target");
}

void expandMemberName(jref member_name)
{
    assert(member_name != nullptr);

    jvm_abort("expandMemberName");
}

//static int polymorphicNameID(Class *clazz, const char *name) {
//    if(clazz->class_name == S(java_lang_invoke_MethodHandle) || clazz->class_name == S(java_lang_invoke_VarHandle)) {
//        if(name == S(invoke) || name == S(invokeExact))
//            return ID_invokeGeneric;
//        else if(name == S(invokeBasic))
//            return ID_invokeBasic;
//        else if(name == S(linkToVirtual))
//            return ID_linkToVirtual;
//        else if(name == S(linkToStatic))
//            return ID_linkToStatic;
//        else if(name == S(linkToSpecial))
//            return ID_linkToSpecial;
//        else if(name == S(linkToInterface))
//            return ID_linkToInterface;
//    }
//    return -1;
//}

// void *findNativeMethod(const char *class_name, const char *method_name, const char *method_type);

Object *resolveMemberName(jref member_name, Class *caller)
{
    assert(member_name != nullptr);

    jstrref name_str = member_name->getRefField(mn_name_field);
    Class *clazz = member_name->getRefField<ClassObject>(mn_clazz_field)->jvm_mirror;
    jref type = member_name->getRefField(mn_type_field);
    jint flags = member_name->getIntField(mn_flags_field);

    if(clazz == nullptr || name_str == nullptr || type == nullptr) {
        jvm_abort("resolveMemberName"); // todo
        signalException(S(java_lang_IllegalArgumentException), nullptr);
        return nullptr;
    }

    const utf8_t *name = save(name_str->toUtf8());
    if (name == S(class_init)) {
        jvm_abort("11111111111"); // todo
    }

    jstrref sig_str = RSLOT(execJavaFunc(mn_getSignature_method, { member_name }));
    const utf8_t *sig = sig_str->toUtf8();

    auto xx = toMethodDescriptor(type)->toUtf8();

    switch(flags & ALL_KINDS) {
        case IS_METHOD: {
            Method *m = clazz->lookupMethod(name, sig);
            if (m == nullptr) {
                // m = lookupPolymorphicMethod();
                // m = findNativeMethod(clazz->class_name, m->name, nullptr);

                // todo
                m = clazz->getDeclaredPolymorphicSignatureMethod(name); 
                // jvm_abort("not implement"); // todo
            }
            if (m == nullptr) {
                jvm_abort("error"); // todo
            }

            flags |= methodFlags(m);
            member_name->setIntField(mn_flags_field, flags);
            return member_name;
        }
        case IS_CONSTRUCTOR: {
            jvm_abort("IS_CONSTRUCTOR"); // todo
            return member_name;
        }
        case IS_FIELD: {
            jvm_abort("IS_FIELD"); // todo
            return member_name;
        }
        default:
            jvm_abort("error"); // todo
    }

    jvm_abort("resolveMemberName");
}

/* ----------------------------------------------------------------------------------------- */

jref method_handles::getCaller()
{
    // public static Lookup lookup();
    Class *mh = loadBootClass("java/lang/invoke/MethodHandles");
    auto lookup = mh->getDeclaredStaticMethod("lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
    return RSLOT(execJavaFunc(lookup));
}