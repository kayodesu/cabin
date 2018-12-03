/*
 * Author: Jia Yang
 */

#include <string.h>
#include "../jvm.h"
#include "constant.h"
#include "../util/encoding.h"
#include "attribute.h"

static struct element_value* parse_element_value(struct bytecode_reader *reader);

static struct annotation* parse_annotation(struct bytecode_reader *reader)
{
    VM_MALLOC(struct annotation, a);

    a->type_index = bcr_readu2(reader);
    a->num_element_value_pairs = bcr_readu2(reader);
    a->element_value_pairs = malloc(sizeof(struct element_value_pair) * a->num_element_value_pairs); // todo NULL
    for (int i = 0; i < a->num_element_value_pairs; i++) {
        a->element_value_pairs[i].element_name_index = bcr_readu2(reader);
        a->element_value_pairs[i].value = parse_element_value(reader);
    }

    return a;
}

static struct element_value* parse_element_value(struct bytecode_reader *reader)
{
    VM_MALLOC(struct element_value, ev);

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
            ev->value.const_value_index = bcr_readu2(reader);
            break;
        case 'c':
            ev->value.class_info_index = bcr_readu2(reader);
            break;
        case 'e':
            ev->value.enum_const_value.type_name_index = bcr_readu2(reader);
            ev->value.enum_const_value.const_name_index = bcr_readu2(reader);
            break;
        case '@':
            ev->value.annotation_value = parse_annotation(reader);
            break;
        case '[':
            ev->value.array_value.num_values = bcr_readu2(reader);
            ev->value.array_value.values = malloc(sizeof(struct element_value *) * ev->value.array_value.num_values);
            for (int i = 0; i < ev->value.array_value.num_values; i++) {
                ev->value.array_value.values[i] = parse_element_value(reader);
            }
            break;
        default:
            jvm_abort("error\n"); // todo error
    }

    return ev;
}

static void annotation_default_attribute_destroy(void *attr)
{
    // todo
}

static struct annotation_default_attribute*
parse_annotation_default_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct annotation_default_attribute, a);
    a->default_value = parse_element_value(reader);

    a->common.name = AnnotationDefault;
    a->common.attribute_length = attribute_length;
    a->common.destroy = annotation_default_attribute_destroy;
    return a;
}

static void bootstrap_methods_attribute_destroy(void *attr)
{
    // todo
}

static struct bootstrap_methods_attribute*
parse_bootstrap_method_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct bootstrap_methods_attribute, a);
    a->num_bootstrap_methods = bcr_readu2(reader);
    a->bootstrap_methods = malloc(sizeof(struct bootstrap_method) * a->num_bootstrap_methods);

    for (int i = 0; i < a->num_bootstrap_methods; i++) {
        struct bootstrap_method *bm = a->bootstrap_methods + i;
        bm->bootstrap_method_ref = bcr_readu2(reader);
        bm->num_bootstrap_arguments = bcr_readu2(reader);
        bm->bootstrap_arguments = malloc(sizeof(u2) * bm->num_bootstrap_arguments);
        for (int j = 0; j < bm->num_bootstrap_arguments; j++) {
            bm->bootstrap_arguments[j] = bcr_readu2(reader);
        }
    }

    a->common.name = BootstrapMethods;
    a->common.attribute_length = attribute_length;
    a->common.destroy = bootstrap_methods_attribute_destroy;
    return a;
}

static void code_attribute_destroy(void *attr)
{
    // todo
}

static struct code_attribute*
parse_code_attribute(struct bytecode_reader *reader, u4 attribute_length, void **constant_pool, u2 constant_pool_count)
{
    VM_MALLOC(struct code_attribute, a);

    a->max_stack = bcr_readu2(reader);
    a->max_locals = bcr_readu2(reader);
    a->code_length = bcr_readu4(reader);
    a->code = malloc(sizeof(u1) * a->code_length);
    for (int i = 0; i < a->code_length; i++) {
        a->code[i] = bcr_readu1(reader);  // todo 严重效率问题，应该一次读入。
    }

    a->exception_tables_length = bcr_readu2(reader);
    a->exception_tables = malloc(sizeof(struct code_attribute_exception_table) * a->exception_tables_length);
    for (int i = 0; i < a->exception_tables_length; i++) {
        a->exception_tables[i].start_pc = bcr_readu2(reader);
        a->exception_tables[i].end_pc = bcr_readu2(reader);
        a->exception_tables[i].handler_pc = bcr_readu2(reader);
        a->exception_tables[i].catch_type = bcr_readu2(reader);
    }

    a->attributes_count = bcr_readu2(reader);
    a->attributes = malloc(sizeof(struct attribute_common *) * a->attributes_count);
    for (int i = 0; i < a->attributes_count; i++) {
        a->attributes[i] = parse_attribute(reader, constant_pool, constant_pool_count);
    }

    a->common.name = Code;
    a->common.attribute_length = attribute_length;
    a->common.destroy = code_attribute_destroy;
    return a;
}

static void constant_value_attribute_destroy(void *attr)
{
    // todo
}

static struct constant_value_attribute*
parse_constant_value_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct constant_value_attribute, a);
    a->constant_value_index = bcr_readu2(reader);

    a->common.name = ConstantValue;
    a->common.attribute_length = attribute_length;
    a->common.destroy = constant_value_attribute_destroy;
    return a;
}

static void deprecated_attribute_destroy(void *attr)
{
    // todo
}

static struct deprecated_attribute*
parse_deprecated_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct deprecated_attribute, a);

    a->common.name = Deprecated;
    a->common.attribute_length = attribute_length;
    a->common.destroy = deprecated_attribute_destroy;
    return a;
}

static void enclosing_method_attribute_destroy(void *attr)
{
    // todo
}

static struct enclosing_method_attribute*
parse_enclosing_method_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct enclosing_method_attribute, a);

    a->class_index = bcr_readu2(reader);
    a->method_index = bcr_readu2(reader);

    a->common.name = EnclosingMethod;
    a->common.attribute_length = attribute_length;
    a->common.destroy = enclosing_method_attribute_destroy;
    return a;
}

static void exceptions_attribute_destroy(void *attr)
{
    // todo
}

static struct exceptions_attribute*
parse_exceptions_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct exceptions_attribute, a);

    a->number_of_exceptions = bcr_readu2(reader);
    a->exception_index_table = malloc(sizeof(u2) * a->number_of_exceptions);
    for (int i = 0; i < a->number_of_exceptions; i++) {
        a->exception_index_table[i] = bcr_readu2(reader);
    }

    a->common.name = Exceptions;
    a->common.attribute_length = attribute_length;
    a->common.destroy = exceptions_attribute_destroy;
    return a;
}

static void inner_classes_attribute_destroy(void *attr)
{
    // todo
}

static struct inner_classes_attribute*
parse_inner_classes_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct inner_classes_attribute, a);

    a->number_of_classes = bcr_readu2(reader);
    a->classes = malloc(sizeof(struct inner_class) * a->number_of_classes);
    for (int i = 0; i < a->number_of_classes; i++) {
        a->classes[i].inner_class_info_index = bcr_readu2(reader);
        a->classes[i].outer_class_info_index = bcr_readu2(reader);
        a->classes[i].inner_name_index = bcr_readu2(reader);
        a->classes[i].inner_class_access_flags = bcr_readu2(reader);
    }

    a->common.name = InnerClasses;
    a->common.attribute_length = attribute_length;
    a->common.destroy = inner_classes_attribute_destroy;
    return a;
}

static void line_number_table_attribute_destroy(void *attr)
{
    // todo
}

static struct line_number_table_attribute*
parse_line_number_table_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct line_number_table_attribute, a);

    a->line_number_table_length = bcr_readu2(reader);
    a->line_number_tables = malloc(sizeof(struct line_number_table) * a->line_number_table_length);
    for (int i = 0; i < a->line_number_table_length; i++) {
        a->line_number_tables[i].start_pc = bcr_readu2(reader);
        a->line_number_tables[i].line_number = bcr_readu2(reader);
    }

    a->common.name = LineNumberTable;
    a->common.attribute_length = attribute_length;
    a->common.destroy = line_number_table_attribute_destroy;
    return a;
}

static void local_variable_table_attribute_destroy(void *attr)
{
    // todo
}

static struct local_variable_table_attribute*
parse_local_variable_table_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct local_variable_table_attribute, a);

    a->local_variable_table_length = bcr_readu2(reader);
    a->local_variable_tables = malloc(sizeof(struct local_variable_table) * a->local_variable_table_length);
    for (int i = 0; i < a->local_variable_table_length; i++) {
        a->local_variable_tables[i].start_pc = bcr_readu2(reader);
        a->local_variable_tables[i].length = bcr_readu2(reader);
        a->local_variable_tables[i].name_index = bcr_readu2(reader);
        a->local_variable_tables[i].descriptor_index = bcr_readu2(reader);
        a->local_variable_tables[i].index = bcr_readu2(reader);
    }

    a->common.name = LocalVariableTable;
    a->common.attribute_length = attribute_length;
    a->common.destroy = local_variable_table_attribute_destroy;
    return a;
}

static void local_variable_type_table_attribute_destroy(void *attr)
{
    // todo
}

static struct local_variable_type_table_attribute*
parse_local_variable_type_table_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct local_variable_type_table_attribute, a);

    int len = a->local_variable_type_table_length = bcr_readu2(reader);
    a->local_variable_type_tables = malloc(sizeof(struct local_variable_type_table) * len);
    for (int i = 0; i < len; i++) {
        a->local_variable_type_tables[i].start_pc = bcr_readu2(reader);
        a->local_variable_type_tables[i].length = bcr_readu2(reader);
        a->local_variable_type_tables[i].name_index = bcr_readu2(reader);
        a->local_variable_type_tables[i].signature_index = bcr_readu2(reader);
        a->local_variable_type_tables[i].index = bcr_readu2(reader);
    }

    a->common.name = LocalVariableTypeTable;
    a->common.attribute_length = attribute_length;
    a->common.destroy = local_variable_type_table_attribute_destroy;
    return a;
}


static void method_parameters_attribute_destroy(void *attr)
{
    // todo
}

static struct method_parameters_attribute*
parse_method_parameters_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct method_parameters_attribute, a);

    a->parameters_count = bcr_readu1(reader);
    a->parameters = malloc(sizeof(struct parameter) * a->parameters_count);
    for (int i = 0; i < a->parameters_count; i++) {
        a->parameters[i].name_index = bcr_readu2(reader);
        a->parameters[i].access_flags = bcr_readu2(reader);
    }

    a->common.name = MethodParameters;
    a->common.attribute_length = attribute_length;
    a->common.destroy = method_parameters_attribute_destroy;
    return a;
}

static void runtime_annotations_attribute_destroy(void *attr)
{
    // todo
}

static struct runtime_annotations_attribute*
parse_runtime_annotations_attribute(struct bytecode_reader *reader, u4 attribute_length, bool visible)
{
    VM_MALLOC(struct runtime_annotations_attribute, a);

    a->num_annotations = bcr_readu2(reader);
    a->annotations = malloc(sizeof(struct annotation *) * a->num_annotations);
    for (int i = 0; i < a->num_annotations; i++) {
        a->annotations[i] = parse_annotation(reader);
    }

    a->common.name = visible ? RuntimeVisibleAnnotations : RuntimeInvisibleAnnotations;
    a->common.attribute_length = attribute_length;
    a->common.destroy = runtime_annotations_attribute_destroy;
    return a;
}

static void runtime_parameter_annotations_attribute_destroy(void *attr)
{
    // todo
}

static struct runtime_parameter_annotations_attribute*
parse_runtime_parameter_annotations_attribute(struct bytecode_reader *reader, u4 attribute_length, bool visible)
{
    VM_MALLOC(struct runtime_parameter_annotations_attribute, a);

    a->num_parameters = bcr_readu2(reader);
    a->parameter_annotations = malloc(sizeof(struct parameter_annotation) * a->num_parameters);
    for (int i = 0; i < a->num_parameters; i++) {
        u2 nums = a->parameter_annotations[i].num_annotations = bcr_readu2(reader);
        a->parameter_annotations[i].annotations = malloc(sizeof(struct annotation *) * nums);
        for (int j = 0; j < nums; j++) {
            a->parameter_annotations[i].annotations[j] = parse_annotation(reader);
        }
    }

    a->common.name = visible ? RuntimeVisibleParameterAnnotations : RuntimeInvisibleParameterAnnotations;
    a->common.attribute_length = attribute_length;
    a->common.destroy = runtime_parameter_annotations_attribute_destroy;
    return a;
}

static void signature_attribute_destroy(void *attr)
{
    // todo
}

static struct signature_attribute*
parse_signature_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct signature_attribute, a);

    a->signature_index = bcr_readu2(reader);

    a->common.name = Signature;
    a->common.attribute_length = attribute_length;
    a->common.destroy = signature_attribute_destroy;
    return a;
}

static void source_debug_extension_attribute_destroy(void *attr)
{
    // todo
}

static struct source_debug_extension_attribute*
parse_source_debug_extension_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct source_debug_extension_attribute, a);

    a->debug_extension = malloc(sizeof(u1) * attribute_length);

    for (int i = 0; i < attribute_length; i++) {
        a->debug_extension[i] = bcr_readu1(reader); // todo 效率低下，应该一次读取
    }

    a->common.name = SourceDebugExtension;
    a->common.attribute_length = attribute_length;
    a->common.destroy = source_debug_extension_attribute_destroy;
    return a;
}

static void source_file_attribute_destroy(void *attr)
{
    // todo
}

static struct source_file_attribute*
parse_source_file_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct source_file_attribute, a);

    a->source_file_index = bcr_readu2(reader);

    a->common.name = SourceFile;
    a->common.attribute_length = attribute_length;
    a->common.destroy = source_file_attribute_destroy;
    return a;
}

static void stack_map_table_attribute_destroy(void *attr)
{
    // todo
}

static struct stack_map_table_attribute*
parse_stack_map_table_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct stack_map_table_attribute, a);

    a->number_of_entries = bcr_readu2(reader);
    // 跳过剩下的部分，先不处理。  todo
    bcr_skip(reader, (int) attribute_length - 2);

    a->common.name = StackMapTable;
    a->common.attribute_length = attribute_length;
    a->common.destroy = stack_map_table_attribute_destroy;
    return a;
}

static void synthetic_attribute_destroy(void *attr)
{
    // todo
}

static struct synthetic_attribute*
parse_synthetic_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct synthetic_attribute, a);

    a->common.name = Synthetic;
    a->common.attribute_length = attribute_length;
    a->common.destroy = synthetic_attribute_destroy;
    return a;
}

static void unknown_attribute_destroy(void *attr)
{
    // todo
}

static struct unknown_attribute*
parse_unknown_attribute(struct bytecode_reader *reader, u4 attribute_length)
{
    VM_MALLOC(struct unknown_attribute, a);

    bcr_skip(reader, attribute_length);

    a->common.name = ""; // todo
    a->common.attribute_length = attribute_length;
    a->common.destroy = unknown_attribute_destroy;
    return a;
}


void* parse_attribute(struct bytecode_reader *reader, void **constant_pool, u2 constant_pool_count)
{
    u2 attribute_name_index = bcr_readu2(reader);
    u4 attribute_length = bcr_readu4(reader);

    if (attribute_name_index < 1 || attribute_name_index > constant_pool_count - 1) {
        // todo error
        jvm_abort("error");
    }

    void *constant = constant_pool[attribute_name_index];
    if (constant == NULL) {
        // todo
        jvm_abort("error\n");
    }

    if (CONSTANT_TAG(constant) != UTF8_CONSTANT) {
        //todo
        jvm_abort("error. %d\n", CONSTANT_TAG(constant));
    }

    struct utf8_constant *uc = (struct utf8_constant *)constant;
    char attr_name[uc->length + 1];
    decode_mutf8(uc->bytes, uc->length, attr_name);

    if (strcmp(Code, attr_name) == 0)
        return parse_code_attribute(reader, attribute_length, constant_pool, constant_pool_count);
    if (strcmp(ConstantValue, attr_name) == 0)
        return parse_constant_value_attribute(reader, attribute_length);
    if (strcmp(Signature, attr_name) == 0)
        return parse_signature_attribute(reader, attribute_length);
    if (strcmp(Synthetic, attr_name) == 0)
        return parse_synthetic_attribute(reader, attribute_length);
    if (strcmp(Deprecated, attr_name) == 0)
        return parse_deprecated_attribute(reader, attribute_length);
    if (strcmp(LineNumberTable, attr_name) == 0)
        return parse_line_number_table_attribute(reader, attribute_length);
    if (strcmp(StackMapTable, attr_name) == 0)
        return parse_stack_map_table_attribute(reader, attribute_length);
    if (strcmp(LocalVariableTable, attr_name) == 0)
        return parse_local_variable_table_attribute(reader, attribute_length);
    if (strcmp(LocalVariableTypeTable, attr_name) == 0)
        return parse_local_variable_type_table_attribute(reader, attribute_length);
    if (strcmp(Exceptions, attr_name) == 0)
        return parse_exceptions_attribute(reader, attribute_length);
    if (strcmp(RuntimeVisibleParameterAnnotations, attr_name) == 0)
        return parse_runtime_parameter_annotations_attribute(reader, attribute_length, true);
    if (strcmp(RuntimeInvisibleParameterAnnotations, attr_name) == 0)
        return parse_runtime_parameter_annotations_attribute(reader, attribute_length, false);
    if (strcmp(AnnotationDefault, attr_name) == 0)
        return parse_annotation_default_attribute(reader, attribute_length);
    if (strcmp(SourceFile, attr_name) == 0)
        return parse_source_file_attribute(reader, attribute_length);
    if (strcmp(InnerClasses, attr_name) == 0)
        return parse_inner_classes_attribute(reader, attribute_length);
    if (strcmp(EnclosingMethod, attr_name) == 0)
        return parse_enclosing_method_attribute(reader, attribute_length);
    if (strcmp(MethodParameters, attr_name) == 0)
        return parse_method_parameters_attribute(reader, attribute_length);
    if (strcmp(SourceDebugExtension, attr_name) == 0)
        return parse_source_debug_extension_attribute(reader, attribute_length);
    if (strcmp(RuntimeVisibleAnnotations, attr_name) == 0)
        return parse_runtime_annotations_attribute(reader, attribute_length, true);
    if (strcmp(RuntimeInvisibleAnnotations, attr_name) == 0)
        return parse_runtime_annotations_attribute(reader, attribute_length, false);
    if (strcmp(BootstrapMethods, attr_name) == 0)
        return parse_bootstrap_method_attribute(reader, attribute_length);

    printvm("unknown attr = %s\n", attr_name); // todo
    return parse_unknown_attribute(reader, attribute_length);
}
