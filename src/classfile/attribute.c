/*
 * Author: Jia Yang
 */

#include <string.h>
#include <assert.h>
#include "../jvm.h"
#include "constant.h"
#include "../util/encoding.h"
#include "attribute.h"

static void read_element_value(struct bytecode_reader *reader, struct element_value *ev);

static void read_annotation(struct bytecode_reader *reader, struct annotation *a)
{
    assert(reader != NULL);
    assert(a != NULL);

    a->type_index = bcr_readu2(reader);
    a->num_element_value_pairs = bcr_readu2(reader);
    a->element_value_pairs = malloc(sizeof(struct element_value_pair) * a->num_element_value_pairs);
    CHECK_MALLOC_RESULT(a->element_value_pairs);
    for (int i = 0; i < a->num_element_value_pairs; i++) {
        a->element_value_pairs[i].element_name_index = bcr_readu2(reader);
        a->element_value_pairs[i].value = malloc(sizeof(struct element_value));
        CHECK_MALLOC_RESULT(a->element_value_pairs[i].value);
        read_element_value(reader, a->element_value_pairs[i].value);
    }
}

static void read_element_value(struct bytecode_reader *reader, struct element_value *ev)
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
            read_annotation(reader, &(ev->value.annotation_value));
            break;
        case '[':
            ev->value.array_value.num_values = bcr_readu2(reader);
            ev->value.array_value.values = malloc(sizeof(struct element_value) * ev->value.array_value.num_values);
            CHECK_MALLOC_RESULT(ev->value.array_value.values);
            for (int i = 0; i < ev->value.array_value.num_values; i++) {
                read_element_value(reader, ev->value.array_value.values + i);
            }
            break;
        default:
            VM_UNKNOWN_ERROR("unknown tag: %d", ev->tag);
    }
}

static void read_bootstrap_methods_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_BOOTSTRAP_METHODS;

    u2 num = result->u.bootstrap_methods.num = bcr_readu2(reader);
    result->u.bootstrap_methods.methods = malloc(sizeof(struct bootstrap_method) * num);
    CHECK_MALLOC_RESULT(result->u.bootstrap_methods.methods);

    for (int i = 0; i < num; i++) {
        struct bootstrap_method *bm = result->u.bootstrap_methods.methods + i;
        bm->bootstrap_method_ref = bcr_readu2(reader);
        bm->num_bootstrap_arguments = bcr_readu2(reader);
        bm->bootstrap_arguments = malloc(sizeof(u2) * bm->num_bootstrap_arguments);
        CHECK_MALLOC_RESULT(bm->bootstrap_arguments);
        for (int j = 0; j < bm->num_bootstrap_arguments; j++) {
            bm->bootstrap_arguments[j] = bcr_readu2(reader);
        }
    }
}

static void read_code_attribute(struct bytecode_reader *reader,
                                 struct constant *constant_pool, u2 constant_pool_count,
                                 struct attribute *result)
{
    assert(reader != NULL);
    assert(constant_pool != NULL);
    assert(result != NULL);

    result->type = AT_CODE;

    result->u.code.max_stack = bcr_readu2(reader);
    result->u.code.max_locals = bcr_readu2(reader);
    result->u.code.code_length = bcr_readu4(reader);
    result->u.code.code = malloc(sizeof(u1) * result->u.code.code_length);
    CHECK_MALLOC_RESULT(result->u.code.code);
    bcr_read_bytes(reader, result->u.code.code, result->u.code.code_length);

    u2 len = result->u.code.exception_tables_length = bcr_readu2(reader);
    result->u.code.exception_tables = malloc(sizeof(struct code_attribute_exception_table) * len);
    CHECK_MALLOC_RESULT(result->u.code.exception_tables);
    for (int i = 0; i < len; i++) {
        result->u.code.exception_tables[i].start_pc = bcr_readu2(reader);
        result->u.code.exception_tables[i].end_pc = bcr_readu2(reader);
        result->u.code.exception_tables[i].handler_pc = bcr_readu2(reader);
        result->u.code.exception_tables[i].catch_type = bcr_readu2(reader);
    }

    len = result->u.code.attributes_count = bcr_readu2(reader);
    result->u.code.attributes = malloc(sizeof(struct attribute) * len);
    CHECK_MALLOC_RESULT(result->u.code.attributes);
    for (int i = 0; i < len; i++) {
        read_attribute(reader, constant_pool, constant_pool_count, result->u.code.attributes + i);
    }
}

static void read_exceptions_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_EXCEPTIONS;

    u2 num = result->u.exceptions.num = bcr_readu2(reader);
    result->u.exceptions.exception_index_table = malloc(sizeof(u2) * num);
    CHECK_MALLOC_RESULT(result->u.exceptions.exception_index_table);
    for (int i = 0; i < num; i++) {
        result->u.exceptions.exception_index_table[i] = bcr_readu2(reader);
    }
}

static void read_inner_classes_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_INNER_CLASSES;

    u2 num = result->u.inner_classes.num = bcr_readu2(reader);
    struct inner_class *classes = result->u.inner_classes.classes = malloc(sizeof(struct inner_class) * num);
    CHECK_MALLOC_RESULT(classes);
    for (int i = 0; i < num; i++) {
        classes[i].inner_class_info_index = bcr_readu2(reader);
        classes[i].outer_class_info_index = bcr_readu2(reader);
        classes[i].inner_name_index = bcr_readu2(reader);
        classes[i].inner_class_access_flags = bcr_readu2(reader);
    }
}

static void read_line_number_table_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_LINE_NUMBER_TABLE;

    u2 num = result->u.line_number_table.num = bcr_readu2(reader);
    result->u.line_number_table.tables = malloc(sizeof(struct line_number_table) * num);
    CHECK_MALLOC_RESULT(result->u.line_number_table.tables);
    for (int i = 0; i < num; i++) {
        result->u.line_number_table.tables[i].start_pc = bcr_readu2(reader);
        result->u.line_number_table.tables[i].line_number = bcr_readu2(reader);
    }
}

static void read_local_variable_table_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_LOCAL_VARIABLE_TABLE;

    u2 num = result->u.local_variable_table.num = bcr_readu2(reader);
    struct local_variable_table *tables
            = result->u.local_variable_table.tables = malloc(sizeof(struct local_variable_table) * num);
    CHECK_MALLOC_RESULT(tables);
    for (int i = 0; i < num; i++) {
        tables[i].start_pc = bcr_readu2(reader);
        tables[i].length = bcr_readu2(reader);
        tables[i].name_index = bcr_readu2(reader);
        tables[i].descriptor_index = bcr_readu2(reader);
        tables[i].index = bcr_readu2(reader);
    }
}

static void read_local_variable_type_table_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_LOCAL_VARIABLE_TYPE_TABLE;

    u2 num = result->u.local_variable_type_table.num = bcr_readu2(reader);
    struct local_variable_type_table *tables
            = result->u.local_variable_type_table.tables = malloc(sizeof(struct local_variable_type_table) * num);
    CHECK_MALLOC_RESULT(tables);
    for (int i = 0; i < num; i++) {
        tables[i].start_pc = bcr_readu2(reader);
        tables[i].length = bcr_readu2(reader);
        tables[i].name_index = bcr_readu2(reader);
        tables[i].signature_index = bcr_readu2(reader);
        tables[i].index = bcr_readu2(reader);
    }
}

static void read_method_parameters_attribute(struct bytecode_reader *reader, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_METHOD_PARAMETERS;

    u1 num = result->u.method_parameters.num = bcr_readu1(reader);
    struct parameter *parameters = result->u.method_parameters.parameters = malloc(sizeof(struct parameter) * num);
    CHECK_MALLOC_RESULT(parameters);
    for (int i = 0; i < num; i++) {
        parameters[i].name_index = bcr_readu2(reader);
        parameters[i].access_flags = bcr_readu2(reader);
    }
}

static void read_runtime_annotations_attribute(
        struct bytecode_reader *reader, bool visible, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = visible ? AT_RUNTIME_VISIBLE_ANNOTATIONS : AT_RUNTIME_INVISIBLE_ANNOTATIONS;

    u2 num = result->u.runtime_annotations.num = bcr_readu2(reader);
    struct annotation *annotations
            = result->u.runtime_annotations.annotations = malloc(sizeof(struct annotation) * num);
    CHECK_MALLOC_RESULT(annotations);
    for (int i = 0; i < num; i++) {
        read_annotation(reader, annotations + i);
    }
}

static void read_runtime_parameter_annotations_attribute(
        struct bytecode_reader *reader, bool visible, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = visible ? AT_RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS : AT_RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS;

    u2 num = result->u.runtime_parameter_annotations.num = bcr_readu2(reader);
    struct parameter_annotation *parameter_annotations
            = result->u.runtime_parameter_annotations.parameter_annotations
                      = malloc(sizeof(struct parameter_annotation) * num);
    CHECK_MALLOC_RESULT(parameter_annotations);
    for (int i = 0; i < num; i++) {
        u2 nums = parameter_annotations[i].num_annotations = bcr_readu2(reader);
        parameter_annotations[i].annotations = malloc(sizeof(struct annotation) * nums);
        CHECK_MALLOC_RESULT(parameter_annotations[i].annotations);
        for (int j = 0; j < nums; j++) {
            read_annotation(reader, parameter_annotations[i].annotations + j);
        }
    }
}

static void read_source_debug_extension_attribute(
        struct bytecode_reader *reader, u4 attribute_length, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_SOURCE_DEBUG_EXTENSION;

    result->u.source_debug_extension = malloc(sizeof(u1) * attribute_length);
    CHECK_MALLOC_RESULT(result->u.source_debug_extension);
    bcr_read_bytes(reader, result->u.source_debug_extension, attribute_length);
}

static void read_stack_map_table_attribute(
        struct bytecode_reader *reader, u4 attribute_length, struct attribute *result)
{
    assert(reader != NULL);
    assert(result != NULL);

    result->type = AT_STACK_MAP_TABLE;
    result->u.stack_map_table.number_of_entries = bcr_readu2(reader);
    // 跳过剩下的部分，先不处理。  todo
    bcr_skip(reader, (int) attribute_length - 2);
}

void read_attribute(struct bytecode_reader *reader,
                      struct constant *constant_pool, u2 constant_pool_count,
                      struct attribute *result)
{
    assert(reader != NULL);
    assert(constant_pool != NULL);
    assert(result != NULL);

    u2 attribute_name_index = bcr_readu2(reader);
    u4 attribute_length = bcr_readu4(reader);

    if (attribute_name_index < 1 || attribute_name_index > constant_pool_count - 1) {
        // todo error
        jvm_abort("error");
    }

    struct constant *c = constant_pool + attribute_name_index;
    if (c->tag != UTF8_CONSTANT) {
        //todo
        jvm_abort("error. %d\n", constant_pool[attribute_name_index].tag);
    }

    char attr_name[c->u.utf8_constant.length + 1];
    decode_mutf8(c->u.utf8_constant.bytes, c->u.utf8_constant.length, attr_name);

    if (strcmp(Code, attr_name) == 0) {
        read_code_attribute(reader, constant_pool, constant_pool_count, result);
    } else if (strcmp(ConstantValue, attr_name) == 0) {
        result->type = AT_CONSTANT_VALUE;
        result->u.constant_value_index = bcr_readu2(reader);
    } else if (strcmp(Signature, attr_name) == 0) {
        result->type = AT_SIGNATURE;
        result->u.signature_index = bcr_readu2(reader);
    } else if (strcmp(Synthetic, attr_name) == 0) {
        result->type = AT_SYNTHETIC;
    } else if (strcmp(Deprecated, attr_name) == 0) {
        result->type = AT_DEPRECATED;
    } else if (strcmp(LineNumberTable, attr_name) == 0) {
        read_line_number_table_attribute(reader, result);
    } else if (strcmp(StackMapTable, attr_name) == 0) {
        read_stack_map_table_attribute(reader, attribute_length, result);
    } else if (strcmp(LocalVariableTable, attr_name) == 0) {
        read_local_variable_table_attribute(reader, result);
    } else if (strcmp(LocalVariableTypeTable, attr_name) == 0) {
        read_local_variable_type_table_attribute(reader, result);
    } else if (strcmp(Exceptions, attr_name) == 0) {
        read_exceptions_attribute(reader, result);
    } else if (strcmp(RuntimeVisibleParameterAnnotations, attr_name) == 0) {
        read_runtime_parameter_annotations_attribute(reader, true, result);
    } else if (strcmp(RuntimeInvisibleParameterAnnotations, attr_name) == 0) {
        read_runtime_parameter_annotations_attribute(reader, false, result);
    } else if (strcmp(AnnotationDefault, attr_name) == 0) {
        result->type = AT_ANNOTATION_DEFAULT;
        read_element_value(reader, &(result->u.annotation_default_value));
    } else if (strcmp(SourceFile, attr_name) == 0) {
        result->type = AT_SOURCE_FILE;
        result->u.source_file_index = bcr_readu2(reader);
    } else if (strcmp(InnerClasses, attr_name) == 0) {
        read_inner_classes_attribute(reader, result);
    } else if (strcmp(EnclosingMethod, attr_name) == 0) {
        result->type = AT_ENCLOSING_METHOD;
        result->u.enclosing_method.class_index = bcr_readu2(reader);
        result->u.enclosing_method.method_index = bcr_readu2(reader);
    } else if (strcmp(MethodParameters, attr_name) == 0) {
        read_method_parameters_attribute(reader, result);
    } else if (strcmp(SourceDebugExtension, attr_name) == 0) {
        read_source_debug_extension_attribute(reader, attribute_length, result);
    } else if (strcmp(RuntimeVisibleAnnotations, attr_name) == 0) {
        read_runtime_annotations_attribute(reader, true, result);
    } else if (strcmp(RuntimeInvisibleAnnotations, attr_name) == 0) {
        read_runtime_annotations_attribute(reader, false, result);
    } else if (strcmp(BootstrapMethods, attr_name) == 0) {
        read_bootstrap_methods_attribute(reader, result);
    } else {
        // unknown attribute
        result->type = AT_UNKNOWN;
        bcr_skip(reader, attribute_length);
    }
}
