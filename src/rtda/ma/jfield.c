/*
 * Author: Jia Yang
 */

#include <stdlib.h>
#include "jfield.h"
#include "rtcp.h"
#include "jclass.h"
#include "../../util/util.h"
#include "../heap/jobject.h"


void jfield_init(struct jfield *field, struct jclass *c, struct bytecode_reader *reader)
{
    field->constant_value_index = INVALID_CONSTANT_VALUE_INDEX;
    field->jclass = c;
    field->deprecated = false;
    field->access_flags = bcr_readu2(reader);
    field->name = rtcp_get_str(c->rtcp, bcr_readu2(reader));
    field->descriptor = rtcp_get_str(c->rtcp, bcr_readu2(reader));

    char d = field->descriptor[0];
    if (d == 'J' || d == 'D') {
        field->category_two = true;
    } else {
        field->category_two = false;
    }

    field->type = NULL;

    // parse field's attributes
    u2 attr_count = bcr_readu2(reader);
    for (int i = 0; i < attr_count; i++) {
        const char *attr_name = rtcp_get_str(c->rtcp, bcr_readu2(reader));
        u4 attr_len = bcr_readu4(reader);

        if (strcmp(Deprecated, attr_name) == 0) {
            field->deprecated = true;
        } else if (strcmp(ConstantValue, attr_name) == 0) {
            /*
             * ConstantValue属性表示一个常量字段的值。
             * 在一个field_info结构的属性表中最多只能有一个ConstantValue属性。
             *
             * 非静态字段包含了ConstantValue属性，那么这个属性必须被虚拟机所忽略。
             */
            u2 index = bcr_readu2(reader);
            if (IS_STATIC(field->access_flags)) {  // todo
                field->constant_value_index = index;
            }
        } else if (strcmp(Synthetic, attr_name) == 0) {
            set_synthetic(&field->access_flags);
        } else if (strcmp(Signature, attr_name) == 0) {
            c->signature = rtcp_get_str(c->rtcp, bcr_readu2(reader));
        }
#if 0
        else if (strcmp(RuntimeVisibleAnnotations, attr_name) == 0) { // todo
            u2 num = field->runtime_visible_annotations_num = bcr_readu2(reader);
            field->runtime_visible_annotations = malloc(sizeof(struct annotation) * num);
            CHECK_MALLOC_RESULT(field->runtime_visible_annotations);
            for (u2 k = 0; k < num; k++) {
                read_annotation(reader, field->runtime_visible_annotations + i);
            }
        } else if (strcmp(RuntimeInvisibleAnnotations, attr_name) == 0) { // todo
            u2 num = field->runtime_invisible_annotations_num = bcr_readu2(reader);
            field->runtime_invisible_annotations = malloc(sizeof(struct annotation) * num);
            CHECK_MALLOC_RESULT(field->runtime_invisible_annotations);
            for (u2 k = 0; k < num; k++) {
                read_annotation(reader, field->runtime_invisible_annotations + i);
            }
        }
#endif
        else {
            // unknown attribute
//            printvm("unknown attribute: %s\n", attr_name);
            bcr_skip(reader, attr_len);
        }
    }
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
    } else if (d == 'L' && vm_strend(field->descriptor, ";")) { // 变量类型为非数组引用（类引用）
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

void jfield_release(struct jfield *field)
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
