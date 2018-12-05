/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jfield.h"
#include "rtcp.h"
#include "jclass.h"
#include "../../util/util.h"
#include "../heap/jobject.h"
#include "../primitive_types.h"


struct jfield* jfield_create(struct jclass *c, const struct member_info *info)
{
    assert(c != NULL && info != NULL);

    VM_MALLOC(struct jfield, field);
    field->constant_value_index = INVALID_CONSTANT_VALUE_INDEX;
    field->jclass = c;
    field->access_flags = info->access_flags;
    field->name = rtcp_get_str(c->rtcp, info->name_index);
    field->descriptor = rtcp_get_str(c->rtcp, info->descriptor_index);

    char d = field->descriptor[0];
    field->category_two = false;
    if (d == 'J' || d == 'D') {
        field->category_two = true;
    }

    field->type = NULL;

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
    assert(field != NULL);

    if (field->type != NULL) {
        return field->type;
    }

    char d = *field->descriptor;
    if (d == '[') { // 变量类型为数组
        field->type = classloader_load_class(field->jclass->loader, field->descriptor)->clsobj;
    } else if (d == 'L' && strend(field->descriptor, ";")) { // 变量类型为非数组引用（类引用）
        int len = strlen(field->descriptor);
        char type_name[len];
        strcpy(type_name, field->descriptor + 1); // jump 'L'
        type_name[len - 2] = 0; // set last char(';') is 0
        field->type = classloader_load_class(field->jclass->loader, type_name)->clsobj;
    } else { // 变量类型为基本类型
        const char *type_name = pt_get_class_name_by_descriptor(*(field->descriptor));
        if (type_name != NULL) {
            field->type = classloader_load_class(field->jclass->loader, type_name)->clsobj;
        }
    }

    if (field->type == NULL) {
        jvm_abort("Never goes here. %s\n", field->descriptor); // todo
    }
    return field->type;
}

void jfield_destroy(struct jfield *field)
{
    // todo
}

char* jfield_to_string(const struct jfield *field)
{
#define MAX_LEN 1023 // big enough
    VM_MALLOCS(char, MAX_LEN + 1, result);

    if (field != NULL) {
        int n = snprintf(
                result, MAX_LEN, "field: %s~%s~%s", field->jclass->class_name, field->name, field->descriptor);
        if (n < 0) {
            jvm_abort("snprintf 出错\n"); // todo
        }
        assert(0 <= n && n <= MAX_LEN);
        result[n] = 0;
    } else {
        strcpy(result, "field: NULL");
    }

    return result;
#undef MAX_LEN
}

