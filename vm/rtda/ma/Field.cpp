/*
 * Author: kayo
 */

#include <sstream>
#include "Field.h"
#include "../../symbol.h"
#include "resolve.h"
#include "Class.h"
#include "ConstantPool.h"

using namespace std;

Field::Field(Class *c, BytecodeReader &r): Member(c)
{
    constant_value_index = INVALID_CONSTANT_VALUE_INDEX;
    accessFlags = r.readu2();
    name = CP_UTF8(c->cp, r.readu2());
    descriptor = CP_UTF8(c->cp, r.readu2());

    categoryTwo = descriptor[0] == 'J' || descriptor[0]== 'D';

    // parse field's attributes
    u2 attr_count = r.readu2();
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = CP_UTF8(c->cp, r.readu2());
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
            if (isStatic()) {  // todo
                constant_value_index = index;
//                field->v.static_value.u = resolve_single_constant(c, index);  // todo
            }
        } else if (S(Synthetic) == attr_name) {
            setSynthetic();
        } else if (S(Signature) == attr_name) {
            c->signature = CP_UTF8(c->cp, r.readu2());
        } else if (S(RuntimeVisibleAnnotations) == attr_name) { // ignore
//            u2 num = field->runtime_visible_annotations_num = readu2(reader);
//            field->runtime_visible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(field->runtime_visible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, field->runtime_visible_annotations + i);
//            }
            r.skip(attr_len);
        } else if (S(RuntimeInvisibleAnnotations) == attr_name) { // ignore
//            u2 num = field->runtime_invisible_annotations_num = readu2(reader);
//            field->runtime_invisible_annotations = malloc(sizeof(struct annotation) * num);
//            CHECK_MALLOC_RESULT(field->runtime_invisible_annotations);
//            for (u2 k = 0; k < num; k++) {
//                read_annotation(reader, field->runtime_invisible_annotations + i);
//            }
            r.skip(attr_len);
        } else {
            // unknown attribute
            r.skip(attr_len);
        }
    }
}

Class *Field::getType()
{
    if (type == nullptr) {
        type = clazz->loader->loadClass(descriptorToClassName(descriptor).c_str());
    }

    return type;
}

string Field::toString() const
{
    ostringstream oss;
    oss << clazz->className << "~" << name << "~" << descriptor;
    return oss.str();
}
