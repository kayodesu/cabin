/*
 * Author: Jia Yang
 */

#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include "jfield.h"
#include "rtcp.h"
#include "jclass.h"
#include "../../util/util.h"
#include "../../classfile/attribute.h"


struct jfield* jfield_create(const struct jclass *c, const struct member_info *info)
{
    assert(c != NULL && info != NULL);

    VM_MALLOC(struct jfield, field);
    field->constant_value_index = INVALID_CONSTANT_VALUE_INDEX;
    field->jclass = c;
    field->access_flags = info->access_flags;
    field->name = rtcp_get_str(c->rtcp, info->name_index);
    field->descriptor = rtcp_get_str(c->rtcp, info->descriptor_index);

    field->category_two = false;
    if (field->descriptor[0] == 'B') {
        field->type_name = "java/lang/Byte";
    } else if (field->descriptor[0] == 'C') {
        field->type_name = "java/lang/Character";
    }  else if (field->descriptor[0] == 'I') {
        field->type_name = "java/lang/Integer";
    } else if (field->descriptor[0] == 'S') {
        field->type_name = "java/lang/Short";
    } else if (field->descriptor[0] == 'Z') {
        field->type_name = "java/lang/Boolean";
    } else if (field->descriptor[0] == 'F') {
        field->type_name = "java/lang/Float";
    } else if (field->descriptor[0] == 'J') {
        field->type_name = "java/lang/Long";
        field->category_two = true;
    } else if (field->descriptor[0] == 'D') {
        field->type_name = "java/lang/Double";
        field->category_two = true;
    }  else if (field->descriptor[0] == '[') {
        field->type_name = field->descriptor;
    } else if (field->descriptor[0] == 'L' && strend(field->descriptor, ";")) {
        // todo
        char *tmp = strdup(field->descriptor + 1); // jump 'L'
        tmp[strlen(tmp) - 1] = 0; // set last char(';') is 0
        field->type_name = tmp;
//        string className = descriptor.substr(1, descriptor.size() - 2);  todo
//        field->type_name = className;
    } else {
        jvm_abort("%s\n", field->descriptor); // todo
    }

    // 解析 field 的属性
    for (int j = 0; j < info->attributes_count; j++) {
        struct attribute_common *a = info->attributes[j];

        // todo fields Attributes
        if (strcmp(a->name, Deprecated) == 0) { // 可选属性
            //        printvm("not parse attr: Deprecated\n");
        } else if (strcmp(a->name, ConstantValue) == 0) {
            /*
             * ConstantValue属性表示一个常量字段的值。
             * 在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
             *
             * 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
             */
            if (IS_STATIC(field->access_flags)) {  // todo
                field->constant_value_index = ((struct constant_value_attribute *) a)->constant_value_index;
            }
        } else if (strcmp(a->name, Synthetic) == 0) {
            set_synthetic(&field->access_flags); // todo
        } else if (strcmp(a->name, Signature) == 0) {  // 可选属性
//                SignatureAttr *a = attr;
            //            printvm("not parse attr: Signature\n");
        } else if (strcmp(a->name, RuntimeVisibleAnnotations) == 0) {
//                runtime_annotations_attr *a = attr;
            //       printvm("not parse attr: RuntimeVisibleAnnotations\n");
        } else if (strcmp(a->name, RuntimeInvisibleAnnotations) == 0) {
//                runtime_annotations_attr *a = attr;
            //        printvm("not parse attr: RuntimeInvisibleAnnotations\n");
        }
    }

    return field;
}

bool jfield_is_accessible_to(const struct jfield *field, const struct jclass *visitor)
{
    // todo  实现对不对

    if (!jclass_is_accessible_to(field->jclass, visitor)) {
        return false;
    }

    if (field->jclass == visitor || IS_PUBLIC(field->access_flags))  // todo 对不对
        return true;

    if (IS_PRIVATE(field->access_flags)) {
        return false;
    }

    // 字段是protected，则只有 子类 和 同一个包下的类 可以访问
    if (IS_PROTECTED(field->access_flags)) {
        return jclass_is_subclass_of(visitor, field->jclass) || strcmp(field->jclass->pkg_name, visitor->pkg_name) == 0;
    }

    // 字段有默认访问权限（非public，非protected，也非private），则只有同一个包下的类可以访问
    return strcmp(field->jclass->pkg_name, visitor->pkg_name) == 0;
}

struct jobject* jfield_get_type(struct jfield *field)
{
    printvm("未实现的功能   %s\n", jfield_to_string(field));
//    jvm_abort("\n"); // todo
#if 0
    if (type != nullptr) {
        return type;
    }
    type = jclass->loader->getJclassObjFromPool(typeName);
    return type;
#endif
//    char c = descriptor[0];
//    if (c == 'B')
//        return PRIMITIVE_BYTE;
//    if (c == 'C')
//        return PRIMITIVE_CHAR;
//    if (c == 'D')
//        return PRIMITIVE_DOUBLE;
//    if (c == 'F')
//        return PRIMITIVE_FLOAT;
//    if (c == 'I')
//        return PRIMITIVE_INT;
//    if (c == 'J')
//        return PRIMITIVE_LONG;
//    if (c == 'S')
//        return PRIMITIVE_SHORT;
//    if (c == 'Z')
//        return PRIMITIVE_BOOLEAN;
//    if (c == '[' or c == 'L')
//        return REFERENCE;
//    jvmAbort("error\n");
}

char* jfield_to_string(const struct jfield *field)
{
    if (field == NULL) {
        return "jfield: NULL";
    }

    snprintf(global_buf, GLOBAL_BUF_LEN, "field: %s~%s~%s\n", field->jclass->class_name, field->name, field->descriptor);
    return global_buf;
}

