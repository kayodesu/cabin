#include <algorithm>
#include "prims.h"
#include "../symbol.h"
#include "../util/encoding.h"
#include "../cabin.h"
#include "../metadata/field.h"
#include "object.h"
#include "../metadata/class.h"
#include "../interpreter/interpreter.h"
#include "../slot.h"

using namespace std;
using namespace utf8;
using namespace slot;

/*
 * 基本类型的名称，描述符，等等
 *
 * Author: Yo Ka
 */
static const struct {
    const utf8_t *class_name;
    utf8_t descriptor;
    const utf8_t *array_class_name;
    const utf8_t *wrapper_class_name;
} prims[] = {
        { S(void),    'V', S(array_V), S(java_lang_Void) },
        { S(boolean), 'Z', S(array_Z), S(java_lang_Boolean) },
        { S(byte),    'B', S(array_B), S(java_lang_Byte) },
        { S(char),    'C', S(array_C), S(java_lang_Character) },
        { S(short),   'S', S(array_S), S(java_lang_Short) },
        { S(int),     'I', S(array_I), S(java_lang_Integer) },
        { S(long),    'J', S(array_J), S(java_lang_Long) },
        { S(float),   'F', S(array_F), S(java_lang_Float) },
        { S(double),  'D', S(array_D), S(java_lang_Double) }
};

bool isPrimClassName(const utf8_t *class_name)
{
    assert(class_name != nullptr);
    return find_if(begin(prims), end(prims),
                   [=](auto &prim){ return equals(prim.class_name, class_name); }) != end(prims);
}

bool isPrimDescriptor(utf8_t descriptor)
{
    return find_if(begin(prims), end(prims),
                   [=](auto &prim){ return prim.descriptor == descriptor; }) != end(prims);
}

bool isPrimWrapperClassName(const utf8_t *class_name)
{
    assert(class_name != nullptr);
    return find_if(begin(prims), end(prims),
                   [=](auto &prim){ return equals(prim.wrapper_class_name, class_name); }) != end(prims);
}

const utf8_t *getPrimArrayClassName(const utf8_t *class_name)
{
    assert(class_name != nullptr);
    for (auto &t : prims) {
        if (equals(t.class_name, class_name))
            return t.array_class_name;
    }
    return nullptr;
}

const utf8_t *getPrimClassName(utf8_t descriptor)
{
    for (auto &t : prims) {
        if (t.descriptor == descriptor)
            return t.class_name;
    }
    return nullptr;
}

const utf8_t *getPrimDescriptor(const utf8_t *wrapper_class_name)
{
    for (auto &t : prims) {
        if (equals(t.wrapper_class_name, wrapper_class_name))
            return &(t.descriptor);
    }
    return nullptr;
}

const utf8_t *getPrimDescriptorByClassName(const utf8_t *class_name)
{
    for (auto &t : prims) {
        if (equals(t.class_name, class_name))
            return &(t.descriptor);
    }
    return nullptr;
}

const slot_t *primObjUnbox(const Object *box)
{
    assert(box != nullptr);

    Class *c = box->clazz;
    if (!c->isPrimWrapperClass()) {
        jvm_abort("error"); // todo
    }

    // value 的描述符就是基本类型的类名。比如，private final boolean value;
    Field *f = c->lookupField(S(value), getPrimDescriptor(c->class_name));
    if (f == nullptr) {
        jvm_abort("error, %s, %s\n", S(value), c->class_name); // todo
    }
    return box->data + f->id;
}

jref voidBox()
{
    Class *c = loadBootClass(S(java_lang_Void));
    return newObject(c);
}

jref byteBox(jbyte x)
{
    Class *c = loadBootClass(S(java_lang_Byte));
    // public static Byte valueOf(byte b);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(B)Ljava/lang/Byte;");
    return getRef(execJavaFunc(m, {islot(x)}));
}

jref boolBox(jbool x)
{
    Class *c = loadBootClass(S(java_lang_Boolean));
    // public static Boolean valueOf(boolean b);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(Z)Ljava/lang/Boolean;");
    return getRef(execJavaFunc(m, {islot(x)}));
}

jref charBox(jchar x)
{
    Class *c = loadBootClass(S(java_lang_Character));
    // public static Character valueOf(char c);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(C)Ljava/lang/Character;");
    return getRef(execJavaFunc(m, {islot(x)}));
}

jref shortBox(jshort x)
{
    Class *c = loadBootClass(S(java_lang_Short));
    // public static Short valueOf(short s);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(S)Ljava/lang/Short;");
    return getRef(execJavaFunc(m, {islot(x)}));
}


jref intBox(jint x)
{
    Class *c = loadBootClass(S(java_lang_Integer));
    // public static Integer valueOf(int i);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(I)Ljava/lang/Integer;");
    return getRef(execJavaFunc(m, {islot(x)}));
}

jref floatBox(jfloat x)
{
    Class *c = loadBootClass(S(java_lang_Float));
    // public static Float valueOf(float f);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(F)Ljava/lang/Float;");
    return getRef(execJavaFunc(m, {fslot(x)}));
}

jref longBox(jlong x)
{
    Class *c = loadBootClass(S(java_lang_Long));
    // public static Long valueOf(long f);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(J)Ljava/lang/Long;");
    slot_t args[2];
    setLong(args, x);
    return getRef(execJavaFunc(m, args));
}

jref doubleBox(jdouble x)
{
    Class *c = loadBootClass(S(java_lang_Double));
    // public static Double valueOf(double d);
    Method *m = c->getDeclaredStaticMethod(S(valueOf), "(D)Ljava/lang/Double;");
    slot_t args[2];
    setDouble(args, x);
    return getRef(execJavaFunc(m, args));
}
