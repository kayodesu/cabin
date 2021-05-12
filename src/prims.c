#include "../util/encoding.h"

/*
 * 基本类型的名称，描述符，等等
 */

#define PRIMS_COUNT 9

static struct prim {
    const utf8_t *class_name;
    utf8_t descriptor;
    const utf8_t *array_class_name;
    const utf8_t *wrapper_class_name;
} prims[PRIMS_COUNT];
//  = {
//         { S(void),    'V', S(array_V), S(java_lang_Void) },
//         { S(boolean), 'Z', S(array_Z), S(java_lang_Boolean) },
//         { S(byte),    'B', S(array_B), S(java_lang_Byte) },
//         { S(char),    'C', S(array_C), S(java_lang_Character) },
//         { S(short),   'S', S(array_S), S(java_lang_Short) },
//         { S(int),     'I', S(array_I), S(java_lang_Integer) },
//         { S(long),    'J', S(array_J), S(java_lang_Long) },
//         { S(float),   'F', S(array_F), S(java_lang_Float) },
//         { S(double),  'D', S(array_D), S(java_lang_Double) }
// };

void init_prims()
{
    prims[0] = (struct prim){S(void),    'V', S(array_V), S(java_lang_Void) };
    prims[1] = (struct prim){S(boolean), 'Z', S(array_Z), S(java_lang_Boolean)};
    prims[2] = (struct prim){S(byte),    'B', S(array_B), S(java_lang_Byte)};
    prims[3] = (struct prim){S(char),    'C', S(array_C), S(java_lang_Character)};
    prims[4] = (struct prim){S(short),   'S', S(array_S), S(java_lang_Short)};
    prims[5] = (struct prim){S(int),     'I', S(array_I), S(java_lang_Integer)};
    prims[6] = (struct prim){S(long),    'J', S(array_J), S(java_lang_Long) };
    prims[7] = (struct prim){S(float),   'F', S(array_F), S(java_lang_Float)};
    prims[8] = (struct prim){S(double),  'D', S(array_D), S(java_lang_Double)};
}

bool is_prim_class_name(const utf8_t *class_name)
{
    assert(class_name != NULL);

    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (utf8_equals(prims[i].class_name, class_name))
            return true;
    }
    return false;
    // return find_if(begin(prims), end(prims),
    //                [=](auto &prim) { return utf8_equals(prim.class_name, class_name); }) != end(prims);
}

bool is_prim_descriptor(utf8_t descriptor)
{
    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (prims[i].descriptor == descriptor)
            return true;
    }
    return false;
    // return find_if(begin(prims), end(prims),
    //                [=](auto &prim){ return prim.descriptor == descriptor; }) != end(prims);
}

bool is_prim_wrapper_class_name(const utf8_t *class_name)
{
    assert(class_name != NULL);

    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (utf8_equals(prims[i].wrapper_class_name, class_name))
            return true;
    }
    return false;
    // return find_if(begin(prims), end(prims),
    //                [=](auto &prim){ return utf8_equals(prim.wrapper_class_name, class_name); }) != end(prims);
}

const utf8_t *get_prim_array_class_name(const utf8_t *class_name)
{
    assert(class_name != NULL);
    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (utf8_equals(prims[i].class_name, class_name))
            return prims[i].array_class_name;
    }
    return NULL;
}

const utf8_t *get_prim_class_name(utf8_t descriptor)
{
    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (prims[i].descriptor == descriptor)
            return prims[i].class_name;
    }
    return NULL;
}

const utf8_t *get_prim_descriptor_by_wrapper_class_name(const utf8_t *wrapper_class_name)
{
    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (utf8_equals(prims[i].wrapper_class_name, wrapper_class_name))
            return &(prims[i].descriptor);
    }
    return NULL;
}

const utf8_t *get_prim_descriptor_by_class_name(const utf8_t *class_name)
{
    for (size_t i = 0; i < PRIMS_COUNT; i++) {
        if (utf8_equals(prims[i].class_name, class_name))
            return &(prims[i].descriptor);
    }
    return NULL;
}

// const slot_t *primObjUnbox(const Object *box)
// {
//     assert(box != NULL);

//     Class *c = box->clazz;
//     if (!is_prim_wrapper_class(c)) {
//         JVM_PANIC("error"); // todo
//     }

//     // value 的描述符就是基本类型的类名。比如，private final boolean value;
//     Field *f = lookup_field(c, S(value), getPrimDescriptor(c->class_name));
//     if (f == NULL) {
//         JVM_PANIC("error, %s, %s\n", S(value), c->class_name); // todo
//     }
//     return box->data + f->id;
// }

jref voidBox()
{
    Class *c = load_boot_class(S(java_lang_Void));
    return alloc_object(c);
}

jref byteBox(jbyte x)
{
    Class *c = load_boot_class(S(java_lang_Byte));
    // public static Byte valueOf(byte b);
    Method *m = get_declared_static_method(c, S(valueOf), "(B)Ljava/lang/Byte;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {islot(x)}));
}

jref boolBox(jbool x)
{
    Class *c = load_boot_class(S(java_lang_Boolean));
    // public static Boolean valueOf(boolean b);
    Method *m = get_declared_static_method(c, S(valueOf), "(Z)Ljava/lang/Boolean;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {islot(x)}));
}

jref charBox(jchar x)
{
    Class *c = load_boot_class(S(java_lang_Character));
    // public static Character valueOf(char c);
    Method *m = get_declared_static_method(c, S(valueOf), "(C)Ljava/lang/Character;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {islot(x)}));
}

jref shortBox(jshort x)
{
    Class *c = load_boot_class(S(java_lang_Short));
    // public static Short valueOf(short s);
    Method *m = get_declared_static_method(c, S(valueOf), "(S)Ljava/lang/Short;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {islot(x)}));
}

jref intBox(jint x)
{
    Class *c = load_boot_class(S(java_lang_Integer));
    // public static Integer valueOf(int i);
    Method *m = get_declared_static_method(c, S(valueOf), "(I)Ljava/lang/Integer;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {islot(x)}));
}

jref floatBox(jfloat x)
{
    Class *c = load_boot_class(S(java_lang_Float));
    // public static Float valueOf(float f);
    Method *m = get_declared_static_method(c, S(valueOf), "(F)Ljava/lang/Float;");
    return slot_get_ref(exec_java_func(m, (slot_t []) {fslot(x)}));
}

jref longBox(jlong x)
{
    Class *c = load_boot_class(S(java_lang_Long));
    // public static Long valueOf(long f);
    Method *m = get_declared_static_method(c, S(valueOf), "(J)Ljava/lang/Long;");
    slot_t args[2];
    slot_set_long(args, x);
    return exec_java_func_r(m, args);
}

jref doubleBox(jdouble x)
{
    Class *c = load_boot_class(S(java_lang_Double));
    // public static Double valueOf(double d);
    Method *m = get_declared_static_method(c, S(valueOf), "(D)Ljava/lang/Double;");
    slot_t args[2];
    slot_set_double(args, x);
    return exec_java_func_r(m, args);
}
