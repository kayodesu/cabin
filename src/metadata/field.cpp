#include "field.h"
#include "class.h"
#include "../objects/prims.h"

using namespace std;
using namespace utf8;

Field::Field(Class *c, BytecodeReader &r)
{
    assert(c != nullptr);
    clazz = c;
    ConstantPool &cp = c->cp;

    access_flags = r.readu2();
    name = cp.utf8(r.readu2());
    descriptor = cp.utf8(r.readu2());

    category_two = (descriptor[0] == 'J' || descriptor[0]== 'D');

    if (isStatic()) {
        memset(&static_value, 0, sizeof(static_value));
    } else {
        id = -1;
    }

    // parse field's attributes
    u2 attr_count = r.readu2();
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = cp.utf8(r.readu2());
        u4 attr_len = r.readu4();

        if (S(Deprecated) == attr_name) {
            deprecated = true;
        } else if (S(ConstantValue) == attr_name) {
            /*
             * ConstantValue属性表示一个常量字段的值。
             * 在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
             *
             * 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
             */
            u2 index = r.readu2();
            if (isStatic()) {
                utf8_t d = *descriptor;
                if (d == 'Z') {
                    static_value.z = jint2jbool(cp.getInt(index));
                } else if (d == 'B') {
                    static_value.b = jint2jbyte(cp.getInt(index));
                } else if (d == 'C') {
                    static_value.c = jint2jchar(cp.getInt(index));
                } else if (d == 'S') {
                    static_value.s = jint2jshort(cp.getInt(index));
                } else if (d == 'I') {
                    static_value.i = cp.getInt(index);
                } else if (d == 'J') {
                    static_value.j = cp.getLong(index);
                } else if (d == 'F') {
                    static_value.f = cp.getFloat(index);
                } else if (d == 'D') {
                    static_value.d = cp.getDouble(index);
                } else if(equals(descriptor, S(sig_java_lang_String))) {
                    static_value.r = cp.resolveString(index);
                }
            }
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            signature = cp.utf8(r.readu2());
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rt_visi_annos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rt_invisi_annos.emplace_back(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

ClsObj *Field::getType()
{
    if (type == nullptr) {
        if (*descriptor == '[') { // array
            type = loadClass(clazz->loader, descriptor)->java_mirror;
        } else if (*descriptor == 'L') { // non array Object
            utf8_t buf[strlen(descriptor)];
            buf[strlen(strcpy(buf, descriptor + 1)) - 1] = 0; // don't include the first 'L' and the last ';'
            type = loadClass(clazz->loader, buf)->java_mirror;
        } else { // primitive
            assert(strlen(descriptor) == 1);
            auto n = getPrimClassName(*descriptor);
            assert(n != nullptr);
            type = loadBootClass(n)->java_mirror;
        }
    }

    return type;
}

bool Field::isPrim() const
{
    return getPrimClassName(*descriptor) != nullptr;
}

string Field::toString() const
{
    ostringstream oss;
    oss << clazz->class_name << "~" << name << "~" << descriptor << "~" << id;
    return oss.str();
}

ostream &operator <<(ostream &os, const Field &field)
{
    os << field.toString() << endl;
    return os;
}
