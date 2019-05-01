/*
 * Author: Jia Yang
 */

#include <string.h>
#include <assert.h>
#include "../jvm.h"
#include "constant.h"
#include "../util/encoding.h"
#include "attribute.h"
#include "../rtda/ma/class.h"

void read_annotation(BytecodeReader *reader, struct annotation *a)
{
    assert(reader != NULL);
    assert(a != NULL);

    a->type_index = readu2(reader);
    a->num_element_value_pairs = readu2(reader);
    a->element_value_pairs = vm_malloc(sizeof(struct element_value_pair) * a->num_element_value_pairs);
    for (int i = 0; i < a->num_element_value_pairs; i++) {
        a->element_value_pairs[i].element_name_index = readu2(reader);
        a->element_value_pairs[i].value = vm_malloc(sizeof(struct element_value));
        read_element_value(reader, a->element_value_pairs[i].value);
    }
}

void read_element_value(BytecodeReader *reader, struct element_value *ev)
{
    assert(reader != NULL);
    assert(ev != NULL);

    ev->tag = bcr_readu1(reader);
    switch (ev->tag) {
        case 'B':
        case 'C':
        case 'D':
        case 'F':
        case 'I':
        case 'S':
        case 'Z':
        case 's':
            ev->value.const_value_index = readu2(reader);
            break;
        case 'c':
            ev->value.class_info_index = readu2(reader);
            break;
        case 'e':
            ev->value.enum_const_value.type_name_index = readu2(reader);
            ev->value.enum_const_value.const_name_index = readu2(reader);
            break;
        case '@':
            read_annotation(reader, &(ev->value.annotation_value));
            break;
        case '[':
            ev->value.array_value.num_values = readu2(reader);
            ev->value.array_value.values = vm_malloc(sizeof(struct element_value) * ev->value.array_value.num_values);
            for (int i = 0; i < ev->value.array_value.num_values; i++) {
                read_element_value(reader, ev->value.array_value.values + i);
            }
            break;
        default:
            VM_UNKNOWN_ERROR("unknown tag: %d", ev->tag);
    }
}

//struct code_attribute_exception_table *newExceptionTables(BytecodeReader *reader, Method *method)
//{
//    assert(reader != NULL);
//    assert(method != NULL);
//
//    method->exception_tables_count = readu2(reader);
//    if (method->exception_tables_count == 0)
//        return NULL;
//
//    method->exception_tables = vm_malloc(sizeof(struct code_attribute_exception_table) * method->exception_tables_count);
//    for (int i = 0; i < method->exception_tables_count; i++) {
//        struct code_attribute_exception_table *t = method->exception_tables + i;
//        t->start_pc = readu2(reader);
//        t->end_pc = readu2(reader);
//        t->handler_pc = readu2(reader);
//        u2 catch_type = readu2(reader);
//        if (catch_type == 0) {
//            // 异常处理项的 catch_type 有可能是 0。
//            // 0 是无效的常量池索引，但是在这里 0 并非表示 catch-none，而是表示 catch-all。
//            t->catch_type = NULL;
//        } else {
//            if (CP_TYPE(&method->clazz->constant_pool, catch_type) == CONSTANT_ResolvedClass)
//                t->catch_type = (Class *) CP_INFO(&method->clazz->constant_pool, catch_type);
//            else {
//                const char *class_name = CP_CLASS_NAME(&method->clazz->constant_pool, catch_type);
//                t->catch_type = load_class(method->clazz->loader, class_name);
//            }
//        }
//    }
//}

LineNumberTable *newLineNumberTables(BytecodeReader *reader, u2 *line_number_tables_count)
{
    assert(reader != NULL);
    assert(line_number_tables_count != NULL);

    *line_number_tables_count = readu2(reader);
    LineNumberTable *line_number_tables = vm_malloc(sizeof(LineNumberTable) * *line_number_tables_count);
    for (u2 i = 0; i < *line_number_tables_count; i++) {
        line_number_tables[i].start_pc = readu2(reader);
        line_number_tables[i].line_number = readu2(reader);
    }
    return line_number_tables;
}
