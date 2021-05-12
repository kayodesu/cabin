#include "../symbol.h"
#include "../cabin.h"
#include "../util/encoding.h"


static Class *constructor_reflect_class;
static Class *method_reflect_class;
static Class *field_reflect_class;

static Class *member_name_class;
static Field *mn_clazz_field;
static Field *mn_name_field;
static Field *mn_type_field;
static Field *mn_flags_field;
static Field *mn_vmtarget_field;
static Method *mn_getSignature_method;

void init_method_handle()
{
    constructor_reflect_class = load_boot_class(S(java_lang_reflect_Constructor));
    method_reflect_class = load_boot_class(S(java_lang_reflect_Method));
    field_reflect_class = load_boot_class(S(java_lang_reflect_Field));

    Class *c = member_name_class = load_boot_class("java/lang/invoke/MemberName");
    // private Class<?> clazz;
    mn_clazz_field = get_declared_field0(c, S(clazz), S(sig_java_lang_Class));
    // private String name;
    mn_name_field = get_declared_field0(c, S(name), S(sig_java_lang_String));
    // private Object type;
    mn_type_field = get_declared_field0(c, S(type), S(sig_java_lang_Object));
    // private int flags;
    mn_flags_field = get_declared_field0(c, S(flags), S(I));
    // private int flags;
    mn_vmtarget_field = get_declared_field0(c, "vmtarget", S(sig_java_lang_Object));
    // public String getSignature();
    mn_getSignature_method = get_declared_inst_method(c, "getSignature", S(___java_lang_String));

    if (constructor_reflect_class == NULL || method_reflect_class == NULL
        || field_reflect_class == NULL || member_name_class == NULL
        || mn_clazz_field == NULL || mn_name_field == NULL || mn_type_field == NULL
        || mn_flags_field == NULL || mn_vmtarget_field == NULL
        || mn_getSignature_method == NULL) {
        JVM_PANIC("initMethodHandle");  // todo
        // throw runtime_error("initMethodHandle"); // todo
    }
}

jref findMethodType0(jarrref ptypes, jclsref rtype)
{
    assert(ptypes != NULL && rtype != NULL);

    Class *mhn = load_boot_class(S(java_lang_invoke_MethodHandleNatives));
    // static MethodType findMethodHandleType(Class<?> rtype, Class<?>[] ptypes)
    Method *m = get_declared_static_method(mhn, "findMethodHandleType",
                "(Ljava/lang/Class;[Ljava/lang/Class;)Ljava/lang/invoke/MethodType;");
    return slot_get_ref(exec_java_func2(m, rtype, ptypes));
}

jref findMethodType(const utf8_t *desc, jref loader)
{
    assert(desc != NULL);

    jarrref ptypes;
    jref rtype;
    parse_method_descriptor(desc, loader, &ptypes, &rtype);
    return findMethodType0(ptypes, rtype);
}

jref linkMethodHandleConstant(Class *caller_class, int ref_kind,
                              Class *defining_class, const char *name, Object *type)
{
    jref name_str = alloc_string(name);
    Class *mhn = load_boot_class(S(java_lang_invoke_MethodHandleNatives));
    // static MethodHandle linkMethodHandleConstant(Class<?> callerClass, int refKind,
    //                                                 Class<?> defc, String name, Object type)
    Method *m = get_declared_static_method(mhn, "linkMethodHandleConstant",
        "(Ljava/lang/Class;ILjava/lang/Class;Ljava/lang/String;Ljava/lang/Object;)"
                    "Ljava/lang/invoke/MethodHandle;");
    return slot_get_ref(exec_java_func(m,
                               (slot_t []) { rslot(caller_class->java_mirror), islot(ref_kind),
                                 rslot(defining_class->java_mirror), rslot(name_str), rslot(type) }));
}

//Array *method_type::parameterTypes(jref methodType)
//{
//    assert(methodType != NULL);
//
//    // private final Class<?>[] ptypes;
//    auto ptypes = methodType->getRefField<Array>("ptypes", "[Ljava/lang/Class;");
//    return ptypes;
//}

//jstrref method_type::toMethodDescriptor(jref methodType)
//{
//    assert(methodType != NULL);
//
//    Class *mt = loadBootClass("java/lang/invoke/MethodType");
//    // public String toMethodDescriptorString();
//    Method *m = mt->getDeclaredInstMethod("toMethodDescriptorString", "()Ljava/lang/String;");
//    return (jstrref) RSLOT(execJavaFunc(m, {methodType}));
//}

/* ----------------------------------------------------------------------------------------- */

#if 0

jref member_name::memberName(Method *m, jbyte refKind)
{
    assert(m != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public MemberName(Class<?> defClass, String name, MethodType type, byte refKind);
    auto cons = get_constructor(mn, "(Ljava/lang/Class;Ljava/lang/String;Ljava/lang/invoke/MethodType;B)V");
    jref o = alloc_object(mn);
    jref mt = findMethodType(m->descriptor, m->clazz->loader);
    execJavaFunc(cons, { rslot(o), rslot(m->clazz->java_mirror), rslot(alloc_string(m->name)),
                         rslot(mt), islot(refKind) });
    return o;
}

jbyte member_name::getRefKind(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public byte getReferenceKind();
    auto m = get_declared_inst_method(mn, "getReferenceKind", "()B");
    return slot_get_byte(exec_java_func1(m, {member_name}));
}

bool member_name::isMethod(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isMethod();
    auto m = get_declared_inst_method(mn, "isMethod", "()Z");
    return slot_get_bool(exec_java_func1(m, {member_name})) != jfalse;
}

bool member_name::isConstructor(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isConstructor();
    auto m = get_declared_inst_method(mn, "isConstructor", "()Z");
    return slot_get_bool(exec_java_func1(m, {member_name})) != jfalse;
}

bool member_name::isField(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isField();
    auto m = get_declared_inst_method(mn, "isField", "()Z");
    return slot_get_bool(exec_java_func1(m, {member_name})) != jfalse;
}

bool member_name::isType(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isType();
    auto m = get_declared_inst_method(mn, "isType", "()Z");
    return slot_get_bool(exec_java_func1(m, {member_name})) != jfalse;
}

bool member_name::isStatic(jref member_name)
{
    assert(member_name != NULL);

    Class *mn = loadBootClass("java/lang/invoke/MemberName");
    // public boolean isStatic();
    auto m = get_declared_inst_method(mn, "isStatic", "()Z");
    return slot_get_bool(exec_java_func1(m, {member_name})) != jfalse;
}

#endif

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
    assert(member_name != NULL && target != NULL);

    /*
     * in fact, `target` will be one of these three:
     * 1. java/lang/reflect/Field.
     * 2. java/lang/reflect/Constructor.
     * 3. java/lang/reflect/Method.
     */

    if (target->clazz == method_reflect_class) {
        // private Class<?> clazz;
        Class *decl_class = get_ref_field(target, "clazz", "Ljava/lang/Class;")->jvm_mirror;
        // private int slot;
        int slot = get_int_field(target, "slot");

        Method *m = decl_class->methods + slot;
        int flags = methodFlags(m) | IS_METHOD;

        int ref_kind;
        if (ACC_IS_STATIC(m->access_flags)) {
            ref_kind = JVM_REF_invokeStatic;
        } else if (ACC_IS_INTERFACE(decl_class->access_flags)) {
            ref_kind = JVM_REF_invokeInterface;
        } else {
            ref_kind = JVM_REF_invokeVirtual;
        }

        flags |= ref_kind << REFERENCE_KIND_SHIFT;

        set_ref_field0(member_name, mn_clazz_field, decl_class->java_mirror);
        set_int_field0(member_name, mn_flags_field, flags);
        set_ref_field0(member_name, mn_vmtarget_field, (jref) (void *) m);
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
        JVM_PANIC("constructor_reflect_class");
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
        JVM_PANIC("field_reflect_class");
        return;
    }
    JVM_PANIC("initMemberName: unimplemented target"); // todo
    // throw java_lang_InternalError("initMemberName: unimplemented target");
    raise_exception(S(java_lang_InternalError), NULL);  // todo msg
}

void expandMemberName(jref member_name)
{
    assert(member_name != NULL);

    JVM_PANIC("expandMemberName");
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

// void *find_native_method(const char *class_name, const char *method_name, const char *method_type);

Object *resolveMemberName(jref member_name, Class *caller)
{
    assert(member_name != NULL);

    jstrref name_str = get_ref_field0(member_name, mn_name_field);
    Class *clazz = get_ref_field0(member_name, mn_clazz_field)->jvm_mirror;
    jref type = get_ref_field0(member_name, mn_type_field);
    jint flags = get_int_field0(member_name, mn_flags_field);

    if(clazz == NULL || name_str == NULL || type == NULL) {
        JVM_PANIC("resolveMemberName"); // todo
        raise_exception(S(java_lang_IllegalArgumentException), NULL); // todo msg   
        // throw java_lang_IllegalArgumentException();
    }

    const utf8_t *name = save_utf8(string_to_utf8(name_str));
    if (name == S(class_init)) {
        JVM_PANIC("11111111111"); // todo
    }

    jstrref sig_str = slot_get_ref(exec_java_func1(mn_getSignature_method, member_name));
    const utf8_t *sig = string_to_utf8(sig_str);

//    auto xx = toMethodDescriptor(type)->toUtf8();

    switch(flags & ALL_KINDS) {
        case IS_METHOD: {
            Method *m = lookup_method(clazz, name, sig);
            if (m == NULL) {
                // m = lookupPolymorphicMethod();
                // m = find_native_method(clazz->class_name, m->name, NULL);

                // todo
                m = get_declared_poly_method(clazz, name); 
                // JVM_PANIC("not implement"); // todo
            }
            if (m == NULL) {
                // todo
                // throw java_lang_NoSuchMethodError("resolve member name, METHOD");
                raise_exception(S(java_lang_NoSuchMethodError), NULL);  // todo msg
//                JVM_PANIC("error");
            }

            flags |= methodFlags(m);
            set_int_field0(member_name, mn_flags_field, flags);

//            const utf8_t *aa = clazz->class_name;
//            const utf8_t *bb = name;
//            const utf8_t *cc = sig;

            set_ref_field0(member_name, mn_vmtarget_field, (jref) (void *) m);
            return member_name;
        }
        case IS_CONSTRUCTOR: {
            Method *m = lookup_method(clazz, name, sig);
            if (m == NULL) {
                // todo
                // throw java_lang_NoSuchMethodError("resolve member name, CONSTRUCTOR");
                raise_exception(S(java_lang_NoSuchMethodError), NULL);  // todo msg
//                JVM_PANIC("error");
            }

            flags |= methodFlags(m);
            set_int_field0(member_name, mn_flags_field, flags);
            set_ref_field0(member_name, mn_vmtarget_field, (jref) (void *) m);
            return member_name;
        }
        case IS_FIELD: {
            Field *f = lookup_field(clazz, name, sig);
            if (f == NULL) {
                // todo
                // throw java_lang_NoSuchFieldError("resolve member name, FIELD");
                raise_exception(S(java_lang_NoSuchFieldError), NULL);  // todo msg
                return NULL;
//                JVM_PANIC("error");
            }

            flags |= f->access_flags;
            set_int_field0(member_name, mn_flags_field, flags);
            set_ref_field0(member_name, mn_vmtarget_field, (jref) (void *) f);
            return member_name;
        }
        default:
            // throw java_lang_LinkageError("resolve member name");; // todo
            raise_exception(S(java_lang_LinkageError), NULL);  // todo msg
    }

    JVM_PANIC("never reach here"); // todo
}

/* ----------------------------------------------------------------------------------------- */

// jref method_handles::getCaller()
jref getCaller()
{
    // public static Lookup lookup();
    Class *mh = load_boot_class("java/lang/invoke/MethodHandles");
    Method *lookup = get_declared_static_method(mh, "lookup", "()Ljava/lang/invoke/MethodHandles$Lookup;");
    return slot_get_ref(exec_java_func(lookup, NULL));
}

// void java_lang_invoke_MemberName::init()
// {
//     Class *c = loadBootClass("java/lang/invoke/MemberName");
//     // private Class<?> clazz;
// //    mn_clazz_field = c->getDeclaredField(S(clazz), S(sig_java_lang_Class));
//     // private String name;
// //    mn_name_field = c->getDeclaredField(S(name), S(sig_java_lang_String));
//     // private Object type;
// //    mn_type_field = c->getDeclaredField(S(type), S(sig_java_lang_Object));
//     // private int flags;
// //    mn_flags_field = c->getDeclaredField(S(flags), S(I));

//     // private intptr_t   vmindex;     // member index within class or interface
// }
