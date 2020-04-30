/*
 * Author: Yo Ka
 */

#include "field.h"
#include "class.h"
#include "prims.h"

using namespace std;
using namespace utf8;

Field::Field(Class *c, BytecodeReader &r)
{
    assert(c != nullptr);
    clazz = c;
    ConstantPool &cp = c->cp;

    modifiers = r.readu2();
    name = cp.utf8(r.readu2());
    descriptor = cp.utf8(r.readu2());

    category_two = descriptor[0] == 'J' || descriptor[0]== 'D';

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
                    staticValue.z = jint2jbool(cp._int(index));
                } else if (d == 'B') {
                    staticValue.b = jint2jbyte(cp._int(index));
                } else if (d == 'C') {
                    staticValue.c = jint2jchar(cp._int(index));
                } else if (d == 'S') {
                    staticValue.s = jint2jshort(cp._int(index));
                } else if (d == 'I') {
                    staticValue.i = cp._int(index);
                } else if (d == 'J') {
                    staticValue.j = cp._long(index);
                } else if (d == 'F') {
                    staticValue.f = cp._float(index);
                } else if (d == 'D') {
                    staticValue.d = cp._double(index);
                } else if(equals(descriptor, S(sig_java_lang_String))) {
                    staticValue.r = cp.resolveString(index);
                }
            }
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            c->signature = cp.utf8(r.readu2());
        } else if (S(RuntimeVisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rtVisiAnnos.emplace_back(r);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) {
            u2 num = r.readu2();
            for (int j = 0; j < num; j++)
                rtInvisiAnnos.emplace_back(r);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

Class *Field::getType()
{
    if (type == nullptr) {
        if (*descriptor == '[') { // array
            type = loadClass(clazz->loader, descriptor);
        } else if (*descriptor == 'L') { // non array Object
            utf8_t buf[strlen(descriptor)];
            buf[strlen(strcpy(buf, descriptor + 1)) - 1] = 0; // don't include the first 'L' and the last ';'
            type = loadClass(clazz->loader, buf);
        } else { // primitive
            assert(strlen(descriptor) == 1);
            auto name = getPrimClassName(*descriptor);
            assert(name != nullptr);
            type = loadBootClass(name);
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
    oss << clazz->className << "~" << name << "~" << descriptor << "~" << id;
    return oss.str();
}

ostream &operator <<(ostream &os, const Field &field)
{
    os << field.toString() << endl;
    return os;
}
